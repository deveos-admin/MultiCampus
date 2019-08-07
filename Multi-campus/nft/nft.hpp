#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/symbol.hpp>

using namespace eosio;

static const std::string CONTRACT_ACCOUNT = "nftbaymain11";
static const name GAME_CONTRACT = "untowermain1"_n;

typedef uint64_t id_type;

CONTRACT nft : public contract
{
  public:
    nft(name _self, name _code, datastream<const char *> ds) : contract(_self, _code, ds), stokens(_self, _self.value){}

    // eosio.token 에서 transfer 받은 정보를 담기 위한 구조체
    struct nft_transfer
    {
        eosio::name from;
        eosio::name to;
        asset quantity;
        std::string memo;
    };
    ACTION create(name issuer, std::string sym);

    ACTION issue();

    ACTION transferid(name from, name to, id_type id, std::string sym);

    ACTION addwhitelist(name user);

    ACTION deletedata(std::string sym, id_type id);

    ACTION notify(name user, std::string msg);

    void tokenize(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters);
    
    void usercheck(name user);

  private:
    // servant struct
    struct status_info
    {
        uint32_t basic_str = 0;
        uint32_t basic_dex = 0;
        uint32_t basic_int = 0;
    };

    struct servant_info
    {
        uint32_t state;   //서번트 상태
        uint32_t exp = 0; //서번트 경험치
        uint64_t id = 0;
        uint32_t level = 1;
        uint32_t grade = 5;
        status_info status;
        std::vector<uint32_t> equip_slot; //서번트 장비 리스트
        std::vector<uint32_t> passive_skill;
        std::vector<uint32_t> active_skill;
    };

    TABLE account
    {

        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };

    TABLE stats
    {
        asset supply;
        name issuer;

        uint64_t primary_key() const { return supply.symbol.code().raw(); }
        uint64_t get_issuer() const { return issuer.value; }
    };

    TABLE utstoken
    {
        id_type idx;       // Unique 64 bit identifier,
        uint32_t t_idx;    // 유저 테이블 상에서의 고유 인덱스
        std::string state; // 토큰 상태

        name owner;  // token owner
        name master; // token master for search detail info
        asset value; // token value (1 UTS)

        id_type primary_key() const { return idx; }
        uint64_t get_master() const { return master.value; }
        uint64_t get_owner() const { return owner.value; }
    };


    TABLE tservant
    {
        uint64_t index;
        uint32_t party_number = 0;
        servant_info servant;
        uint64_t primary_key() const { return index; }
    };

    TABLE whitelist
    {
        name user;

        uint64_t primary_key() const { return user.value; }
    };

    typedef eosio::multi_index<"accounts"_n, account> account_index;

    typedef eosio::multi_index<"stat"_n, stats, indexed_by<"byissuer"_n, const_mem_fun<stats, uint64_t, &stats::get_issuer>>> currency_index;

    typedef eosio::multi_index<"utstokens"_n, utstoken, indexed_by<"bymaster"_n, const_mem_fun<utstoken, uint64_t, &utstoken::get_master>>, indexed_by<"byowner"_n, const_mem_fun<utstoken, uint64_t, &utstoken::get_owner>>> servant_index;

    typedef eosio::multi_index<"whitelist"_n, whitelist> white_index;

    using servant_table = eosio::multi_index<"tservant"_n, tservant>;

    servant_index stokens;

    void sub_balance(name owner, asset value);
    void add_balance(name owner, asset value, name ram_payer);
    void sub_supply(asset quantity);
    void add_supply(asset quantity);
};