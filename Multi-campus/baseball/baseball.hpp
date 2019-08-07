#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>

#include <string>

using namespace eosio;
using std::string;

CONTRACT baseball : public contract
{
  public:
    name _owner;
    uint64_t num_table[504];

    baseball(name self, name _code, datastream<const char *> ds) : contract(self, _code, ds)
    {
        // Init _owner
        _owner = self;

        // Init Number Table
        uint64_t temp = 0;
        for (uint64_t i = 100; i < 1000; i++)
        {
            if ((i / 10) % 11 != 0)
            {
                if (i / 100 != i % 10)
                {
                    if ((i % 100) % 11 != 0)
                    {
                        if (i % 10 != 0)
                        {
                            if ((i % 100) / 10 != 0)
                            {
                                num_table[temp] = i;
                                temp++;
                            }
                        }
                    }
                }
            }
        }
    }

    ACTION start(name player);

    ACTION throwball(name player, uint64_t value);

    void matchnum(uint64_t num, uint64_t array[]);
    void createnum(name player);

    TABLE status
    {
        name user;
        uint64_t num;
        uint64_t count;
        string text;
        uint64_t primary_key() const { return user.value; }
    };

    typedef eosio::multi_index<"status"_n, status> status_table;
};