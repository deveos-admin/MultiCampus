#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>

#include <string>

using namespace eosio;
    using std::string;

    CONTRACT rsp : public contract
    {
      public:
        name _owner;

        rsp(name _self, name _code, datastream<const char *> ds) : contract(_self, _code, ds)
        {
            _owner = _self;
        }

        ACTION create(name issuer, asset maximum_supply);
        ACTION issue(name to, asset quantity, string memo);
        ACTION transfer(name from, name to, asset quantity, string memo);
        ACTION bet(name payer, asset quantity, string betType);

      private:
        TABLE account
        {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };

        TABLE currencies
        {
            asset supply;
            asset max_supply;
            name issuer;

            uint64_t primary_key() const { return supply.symbol.code().raw(); }
        };

        TABLE score
        {
            name user;
            string text;
            asset amount;

            uint64_t primary_key() const { return user.value; }
        };

        typedef eosio::multi_index<"accounts"_n, account> accounts;
        typedef eosio::multi_index<"stat"_n, currencies> stats;
        typedef eosio::multi_index<"scores"_n, score> scores;

        void sub_balance(name owner, asset value);
        void add_balance(name owner, asset value, name ram_payer);

        void win(name winner, asset value);
        void lose(name looser, asset value);
    };