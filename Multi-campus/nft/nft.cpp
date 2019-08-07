#include "nft.hpp"

void nft::create(name issuer, std::string sym)
{
    require_auth(_self);

    eosio_assert(is_account(issuer), "issuer account does not exist");
    asset supply(0, symbol(sym, 0));

    auto symbol = supply.symbol;
    eosio_assert(symbol.is_valid(), "invalid symbol name");
    eosio_assert(supply.is_valid(), "invalid supply");

    currency_index currency_table(_self, symbol.code().raw());
    auto existing_currency = currency_table.find(symbol.code().raw());
    eosio_assert(existing_currency == currency_table.end(), "token with symbol already exists");

    currency_table.emplace(_self, [&](auto &currency) {
        currency.supply = supply;
        currency.issuer = issuer;
    });
}

void nft::issue()
{
    // transfer로 들어온 정보 데이터를 사용하기 위해 잘라냄. memo에 있는 내용둘이 tokens에 배열로 저장됨.
    auto transfer_data = eosio::unpack_action_data<nft_transfer>();
    std::vector<std::string> tokens;
    tokenize(transfer_data.memo, tokens, ":");

    auto to = transfer_data.from;
    std::string sym = tokens[0];
    // 숫자 데이터로 문자로 들어오기 떄문에 형 변환을 해줘야 함
    const char *tid_char = tokens[1].c_str();
    uint64_t index = std::atoi(tid_char);

    eosio_assert(is_account(to), "to account does not exist");

    // 화이트 리스트에 존재하는 유저인지 체크
    usercheck(to);

    // sym change to symbol type
    asset quantity(1, symbol(symbol_code(sym), 0));
    auto symbols = quantity.symbol;

    eosio_assert(symbols.is_valid(), "invalid symbol name");
    eosio_assert(symbols.precision() == 0, "quantity must be a whole number");

    auto symbol_name = symbols.code().raw();
    currency_index currency_table(_self, symbol_name);

    auto existing_currency = currency_table.find(symbol_name);
    eosio_assert(existing_currency != currency_table.end(), "token with symbol does not exist. create token before issue");
    const auto &st = *existing_currency;

    require_auth(to);
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(symbols == st.supply.symbol, "symbol precision mismatch");

    std::string type = "";

    if (symbols == symbol("UTS", 0))
    {
        type = "servant";
        // 게임 컨트랙트에 있는 정보를 불러옴
        servant_table servant(GAME_CONTRACT, to.value);
        auto servant_iter = servant.get(index, "Not exist Servant");

        // 장비를 착용 하고 있는지 체크
        for (uint8_t i = 0; i < 3; i++)
        {
            eosio_assert(servant_iter.servant.equip_slot[i] == 0, "The servant is wearing the item.");
        }

        // 세컨더리 키(master)로 테이블을 검색하여, 중복된 토큰 데이터가 존재하는지 확인
        auto uts_list = stokens.get_index<"bymaster"_n>();
        auto ute_list_bymaster = uts_list.lower_bound(to.value);
        bool not_exist = true;
        for (auto it = ute_list_bymaster; it != uts_list.end(); ++it)
        {
            if (it->t_idx == servant_iter.index && it->master == to)
            {
                not_exist = false;
                break;
            }
        }
        eosio_assert(not_exist, "Already exist Token");

        stokens.emplace(_self, [&](auto &token) {
            token.idx = stokens.available_primary_key();
            token.t_idx = index;
            token.state = "idle";

            token.owner = to;
            token.master = to;
            token.value = eosio::asset{1, symbols};
        });
    }

    add_supply(quantity);

    add_balance(to, quantity, _self);

    // 인라인 액션으로 게임 컨트랙트에 있는 changetoken 액션을 실행. 권한이 필요하다. eosio.code 권한 설정 필요.
    action(permission_level{get_self(), "active"_n},
           GAME_CONTRACT, "changetoken"_n,
           std::make_tuple(to, type, index))
        .send();
}

void nft::transferid(name from, name to, id_type id, std::string sym)
{
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);

    usercheck(from);

    eosio_assert(is_account(to), "to account does not exist");

    eosio::asset token(0, symbol(symbol_code(sym), 0));

    if (token.symbol == symbol("UTS", 0))
    {
        auto sender_token = stokens.find(id);
        eosio_assert(sender_token != stokens.end(), "token with specified ID does not exist");
        eosio_assert(sender_token->owner == from, "sender does not own token with specified ID");
        eosio_assert(sender_token->state == "idle", "a non-tradeable token");

        const auto &st = *sender_token;

        stokens.modify(st, from, [&](auto &token) {
            token.owner = to;
        });

        sub_balance(from, st.value);
        add_balance(to, st.value, from);
    }

    require_recipient(from);
    require_recipient(to);
}


void nft::deletedata(std::string sym, id_type id)
{
    require_auth(GAME_CONTRACT);

    eosio::asset token(0, symbol(symbol_code(sym), 0));

    if (token.symbol == symbol("UTS", 0))
    {
        auto target_token = stokens.find(id);
        eosio_assert(target_token != stokens.end(), "token with specified ID does not exist");
        eosio_assert(target_token->state == "delete", "Unable to delete in the current state.");

        const auto &st = *target_token;

        stokens.erase(st);
    }
}

void nft::sub_balance(name owner, eosio::asset value)
{
    account_index from_acnts(_self, owner.value);
    const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
    eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

    if (from.balance.amount == value.amount)
    {
        from_acnts.erase(from);
    }
    else
    {
        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }
}

void nft::add_balance(name owner, eosio::asset value, name ram_payer)
{
    account_index to_accounts(_self, owner.value);
    auto to = to_accounts.find(value.symbol.code().raw());
    if (to == to_accounts.end())
    {
        to_accounts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
        });
    }
    else
    {
        to_accounts.modify(to, ram_payer, [&](auto &a) {
            a.balance += value;
        });
    }
}

void nft::sub_supply(eosio::asset quantity)
{
    auto symbol_name = quantity.symbol.code().raw();
    currency_index currency_table(_self, symbol_name);
    auto current_currency = currency_table.find(symbol_name);

    currency_table.modify(current_currency, _self, [&](auto &currency) {
        currency.supply -= quantity;
    });
}

void nft::add_supply(eosio::asset quantity)
{
    auto symbol_name = quantity.symbol.code().raw();
    currency_index currency_table(_self, symbol_name);
    auto current_currency = currency_table.find(symbol_name);

    currency_table.modify(current_currency, _self, [&](auto &currency) {
        currency.supply += quantity;
    });
}

void nft::notify(name user, std::string msg)
{
    require_auth(get_self());
    require_recipient(user);
}

void nft::tokenize(const std::string &str,
                      std::vector<std::string> &tokens,
                      const std::string &delimiters = " ")
{
    // 맨 첫 글자가 구분자인 경우 무시
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // 구분자가 아닌 첫 글자를 찾는다
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // token을 찾았으니 vector에 추가한다
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // 구분자를 뛰어넘는다.  "not_of"에 주의하라
        lastPos = str.find_first_not_of(delimiters, pos);
        // 다음 구분자가 아닌 글자를 찾는다
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void nft::addwhitelist(name user)
{
    require_auth(_code);

    white_index whitelist(_self, _self.value);
    auto white_iter = whitelist.find(user.value);

    eosio_assert(white_iter == whitelist.end(), "This user is already exist in WhiteList.");

    whitelist.emplace(_self, [&](auto &newuser) {
        newuser.user = user;
    });
}

void nft::usercheck(name user)
{
    white_index whitelist(_self, _self.value);
    auto white_iter = whitelist.find(user.value);
    eosio_assert(white_iter != whitelist.end(), "You can't user this contract.");
}

extern "C"
{
    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        if (code == receiver)
        {
            eosio_assert(action != "transfer"_n.value, "Impossible Action");
            switch (action)
            {
                EOSIO_DISPATCH_HELPER(nft, (create)(transferid)(deletedata)(notify)(addwhitelist))
            }
        }
        else if (code == "eosio.token"_n.value && action == "transfer"_n.value)
        {
            execute_action(name(receiver), name(code), &nft::issue);
        }
    }
};
//
// EOSIO_DISPATCH(nft, (create)(issue)(transferid)(notify)