#include "baseball.hpp"

ACTION baseball::start(name player)
{
    baseball::createnum(player);
}

ACTION baseball::throwball(name player, uint64_t value)
{
    status_table status(_self, player.value);
    const auto &data = status.get(player.value, "User not exist");
    eosio_assert(data.count < 9, "Game Over. Please Start new game.");

    uint64_t num = data.num;
    uint64_t strike = 0, ball = 0;
    uint64_t goalarray[3];
    uint64_t playervalue[3];

    matchnum(num, goalarray);
    matchnum(value, playervalue);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (goalarray[i] == playervalue[j] && i == j)
            {
                strike++;
            }
            if (goalarray[i] == playervalue[j] && i != j)
            {
                ball++;
            }
        }
    }

    if (strike == 3)
    {
        status.modify(data, _self, [&](auto &update){
            update.count += 1;
            update.text = "Homerun!! You Win.";
        });
    }
    else
    {
        status.modify(data, _self, [&](auto &update){
            update.count += 1;
            update.text = "Inning : " + std::to_string(update.count) +  "  Strike : " + std::to_string(strike) + "  Ball : " + std::to_string(ball);
        });
    }
}

void baseball::createnum(name player)
{
    capi_checksum256 result;
    uint64_t source = tapos_block_num() * tapos_block_prefix();
    sha256((char *)&source, sizeof(source), &result);
    uint64_t *p = reinterpret_cast<uint64_t *>(&result.hash);
    uint64_t randNum = (*p % 504);

    status_table status(_self, player.value);
    auto iter = status.find(player.value);
    if (iter == status.end())
    {
        status.emplace(_self, [&](auto &data) {
            data.user = player;
            data.num = num_table[randNum];
            data.count = 0;
            data.text = "Play Ball!";
        });
    }
    else
    {
        status.modify(iter, _self, [&](auto &data) {
            data.num = num_table[randNum];
            data.count = 0;
            data.text = "Play Ball!";
        });
    }
}

void baseball::matchnum(uint64_t num, uint64_t array[])
{
    array[0] = num / 100;
    array[1] = num / 10;
    array[1] = array[1] % 10;
    array[2] = num % 10;
}

EOSIO_DISPATCH(baseball, (start)(throwball))