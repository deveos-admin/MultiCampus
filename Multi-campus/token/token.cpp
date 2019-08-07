#include "token.hpp"

void token::create(name issuer, asset maximum_supply)
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

void token::issue(name to, asset quantity, string memo)
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

void token::transfer(name from, name to, asset quantity, string memo)
{
    eosio_assert(from != to, "Cannot transfer to self");
    require_auth(from);
    eosio_assert(is_account(to), "To account does not exist");
    auto sym = quantity.symbol.code().raw();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym, "Not exist symbol");

    accounts from_acnts(_self, from.value);
    auto from_iter = from_acnts.find(sym);
    eosio_assert(from_iter != from_acnts.end(), "Not exist Token in account table.");

    stakes staketable(_self, from.value);
    auto stake_iter = staketable.find(sym);
    eosio_assert(stake_iter != staketable.end(), "Not exist Token in stake table.");

    require_recipient(from);
    require_recipient(to);

    eosio_assert((from_iter->balance.amount - stake_iter->balance.amount) >= quantity.amount, "Not enough Token.");
    eosio_assert(quantity.is_valid(), "Invalid quantity");
    eosio_assert(quantity.amount > 0, "Must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "Symbol precision mismatch");
    eosio_assert(memo.size() <= 250, "Memo has more than 256 bytes");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void token::staking(name who, asset quantity)
{
    require_auth(who);
    auto sym = quantity.symbol.code().raw();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym, "Not exist symbol");

    accounts user_acnts(_self, who.value);
    auto iter = user_acnts.find(sym);
    eosio_assert(iter != user_acnts.end(), "Not exist Token.");

    eosio_assert(iter->balance.amount >= quantity.amount, "Not enough Token.");

    stakes staketable(_self, who.value);
    auto stake_iter = staketable.find(sym);
    if(stake_iter == staketable.end()){
        staketable.emplace(who, [&](auto &data){
            data.balance = quantity;
        });
    }
    else{
        staketable.modify(stake_iter, who, [&](auto &data){
            data.balance += quantity;
        });
    }
}

void token::unstaking(name who, asset quantity)
{
    require_auth(who);
    auto sym = quantity.symbol.code().raw();

    stakes staketable(_self, who.value);
    auto iter = staketable.find(sym);
    eosio_assert(iter != staketable.end(), "Not exist Token in stake table.");
    eosio_assert(iter->balance.amount >= quantity.amount, "Not enough Token.");

    staketable.modify(iter, who, [&](auto &data){
        data.balance -= quantity;
    });
}

void token::sub_balance(name owner, asset value)
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

void token::add_balance(name owner, asset value, name ram_payer)
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

EOSIO_DISPATCH( token, (create)(issue)(transfer)(staking)(unstaking) )