1. EOSIO 설치

- wget https://github.com/EOSIO/eos/releases/download/v1.7.0/eosio_1.7.0-1-ubuntu-18.04_amd64.deb
- sudo apt install ./eosio_1.7.0-1-ubuntu-18.04_amd64.deb

2. EOSIO_CDT 설치

- wget https://github.com/EOSIO/eosio.cdt/releases/download/v1.6.1/eosio.cdt_1.6.1-1_amd64.deb

- sudo apt install ./eosio.cdt_1.6.1-1_amd64.deb

3. Contract 폴더 생성

- mkdir contract // 폴더 생성

- ls // 폴더 확인

- pwd // 폴더 경로

4. Visual Code 설치

5. 경로 확인

- C:\Users\student\AppData\Local\Packages\CanonicalGroupLimited.Ubuntu18.04onWindows_79rhkp1fndgsc\LocalState\rootfs\home

6. EOSIO Contract 불러오기

- git clone https://github.com/EOSIO/eosio.contracts.git

7. 터미널(1) 첫번째 keosd 실행

- keosd

8. 새 터미널(2) 기본 월렛 만들기

- cleos wallet create --to-console

- 패스워드 저장

9. 키 생성

- cleos create key

10. 프라이빗 키 등록

- cleos wallet import --private-key [프라이빗키]

11. 새 터미널(3)에서 첫번째 노드 시작

- nodeos --enable-stale-production --producer-name eosio --plugin eosio::chain_api_plugin --plugin eosio::net_api_plugin

12. 새 터미널(4)에서 eosio.bios 진입

- cd eosio.contracts/contracts/eosio.bios

- mkdir build

- cd build

- mkdir eosio.bios

13. eosio.bios build

- cd eosio.contracts/contracts/eosio.bios/src

- eosio-cpp -I ../include -abigen -o eosio.bios.wasm eosio.bios.cpp

14. eosio.bios 이동

- mv eosio.bios.wasm ../build/eosio.bios

- mv eosio.bios.abi ../build/eosio.bios

15. 터미널(2)에서 set contract

- cd eosio.contracts/contracts/eosio.bios/build

- cleos set contract [계정이름] [배포경로] [.wasm 파일 경로] [.abi 파일 경로]
- cleos set contract eosio eosio.bios --abi eosio.bios.abi

16. 계정 생성

- * EOSIO Key 임포트

- Public : EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
- Private : 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

- 키 생성 -> 키 임포트

- cleos create account eosio [계정명] [퍼블릭키] [퍼블릭키]

17. 터미널(4)에서 노드 실행

- nodeos --enable-stale-production --producer-name [계정명] --plugin eosio::chain_api_plugin --plugin eosio::net_api_plugin --http-server-address 127.0.0.1:8888 --p2p-listen-endpoint 127.0.0.1:9877 --p2p-peer-address 127.0.0.1:9876 --config-dir node2 --data-dir node2 --signature-provider =  [퍼블릭키]=KEY:[프라이빗키]

18. 노드 실패 확인

19. push action

- cleos push action eosio setprods "{ \"schedule\": [{\"producer_name\": \"[계정명]\",\"block_signing_key\": \"[]\"}]}" -p eosio@active

20. 노드 

21. 심화

 - https://developers.eos.io/eosio-nodeos/docs/local-multi-node-testnet
