#include "rsp.hpp"

void rsp::create(name issuer, asset maximum_supply)
{
    require_auth(_self);

    auto sym = maximum_supply.symbol;
    eosio_assert(sym.is_valid(), "Invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "Invalid Supply");
    eosio_assert(maximum_supply.amount > 0, "Max-supply must be positive");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    eosio_assert(existing == statstable.end(), "Tokenwith symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
}

void rsp::issue(name to, asset quantity, string memo)
{
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "Invalid symbol name");
    eosio_assert(memo.size() <= 256, "Memo has more than 256 bytes");

    auto sym_name = sym.code().raw();
    stats statstable(_self, sym_name);
    auto existing = statstable.find(sym_name);
    eosio_assert(existing != statstable.end(), "Token with symbol does now exist, Create token before issue");
    const auto &st = *existing;

    require_auth(st.issuer);
    eosio_assert(quantity.is_valid(), "Invalid quantity");
    eosio_assert(quantity.amount > 0, "Must issue positive quantity");

    eosio_assert(quantity.symbol == st.supply.symbol, "Symbol precision mismatch");
    eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "Quantity exceeds available supply");

    statstable.modify(st, _self, [&](auto &s) {
        s.supply += quantity;
    });

    add_balance(st.issuer, quantity, st.issuer);

    if (to != st.issuer)
    {
        SEND_INLINE_ACTION(*this, transfer, {st.issuer, "active"_n}, {st.issuer, to, quantity, memo});
    }
}

void rsp::transfer(name from, name to, asset quantity, string memo)
{
    eosio_assert(from != to, "Cannot transfer to self");
    require_auth(from);
    eosio_assert(is_account(to), "To account does not exist");
    auto sym = quantity.symbol.code().raw();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym, "Not exist symbol");

    require_recipient(from);
    require_recipient(to);

    eosio_assert(quantity.is_valid(), "Invalid quantity");
    eosio_assert(quantity.amount > 0, "Must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "Symbol precision mismatch");
    eosio_assert(memo.size() <= 250, "Memo has more than 256 bytes");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void rsp::sub_balance(name owner, asset value)
{
    accounts from_acnts(_self, owner.value);

    const auto &from = from_acnts.get(value.symbol.code().raw(), "No balance object found");
    eosio_assert(from.balance.amount >= value.amount, "Overdrawn balance");

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

void rsp::add_balance(name owner, asset value, name ram_payer)
{
    accounts to_acnts(_self, owner.value);
    auto to = to_acnts.find(value.symbol.code().raw());
    if (to == to_acnts.end())
    {
        to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
        });
    }
    else
    {
        to_acnts.modify(to, _self, [&](auto &a) {
            a.balance += value;
        });
    }
}

void rsp::win(name winner, asset value)
{
    asset result = (value * 3);

    scores print_result(_self, winner.value);
    auto existing = print_result.find(winner.value);
    if (existing == print_result.end())
    {
        print_result.emplace(winner, [&](auto &s) {
            s.user = winner;
            s.text = "Win! You get ";
            s.amount = result;
        });
    }
    else
    {
        print_result.modify(existing, _self, [&](auto &s) {
            s.text = "Win! You get ";
            s.amount = result;
        });
    }

    // Add winner's balance
    accounts winner_acnts(_self, winner.value);
    const auto &to = winner_acnts.get(value.symbol.code().raw(), "No balance object found");
    winner_acnts.modify(to, _self, [&](auto &a) {
        a.balance += result;
    });

    // Sub issuer's balance
    accounts owner_acnts(_self, _owner.value);
    const auto &owner = owner_acnts.get(value.symbol.code().raw(), "No balance object found");
    eosio_assert(owner.balance.amount >= result.amount, "error");
    owner_acnts.modify(owner, _self, [&](auto &a) {
        a.balance -= result;
    });
}

void rsp::lose(name looser, asset value)
{
    scores print_result(_self, looser.value);
    auto existing = print_result.find(looser.value);
    if (existing == print_result.end())
    {
        print_result.emplace(looser, [&](auto &s) {
            s.user = looser;
            s.text = "You lost ";
            s.amount = value;
        });
    }
    else
    {
        print_result.modify(existing, _self, [&](auto &s) {
            s.text = "You lost ";
            s.amount = value;
        });
    }
    // Sub looser's balance
    accounts looser_acnts(_self, looser.value);
    const auto &to = looser_acnts.get(value.symbol.code().raw(), "No balance object found");
    looser_acnts.modify(to, _self, [&](auto &a) {
        a.balance -= value;
    });

    //Add issuers's balance
    accounts owner_acnts(_self, _owner.value);
    const auto &owner = owner_acnts.get(value.symbol.code().raw(), "No balance object found");
    owner_acnts.modify(owner, _self, [&](auto &a) {
        a.balance += value;
    });
}

void rsp::bet(name payer, asset quantity, string betType)
{
    accounts from_acnts(_self, payer.value);

    const auto &from = from_acnts.get(quantity.symbol.code().raw(), "No balance object found");

    eosio_assert(from.balance.amount >= quantity.amount, "Overdrawn balance");

    capi_checksum256  result;
    uint64_t source = tapos_block_num() * tapos_block_prefix();
    sha256((char *)&source, sizeof(source), &result);
    uint64_t *p = reinterpret_cast<uint64_t *>(&result.hash);
    uint64_t randNum = *p % 3;

    if (betType == "rock")
    {
        if (2 == randNum)
        {
            win(payer, quantity);
        }
        else
        {
            lose(payer, quantity);
        }
    }
    if (betType == "paper")
    {
        if (0 == randNum)
        {
            win(payer, quantity);
        }
        else
        {
            lose(payer, quantity);
        }
    }
    if (betType == "scissors")
    {
        if (1 == randNum)
        {
            win(payer, quantity);
        }

        else
        {
            lose(payer, quantity);
        }
    }
    else
    {
        print("Parameter Only 'rock', 'paper', 'scissors'.");
    }
}

EOSIO_DISPATCH( rsp, (create)(issue)(transfer)(bet) )