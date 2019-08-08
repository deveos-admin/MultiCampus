### 1. EOSIO Contract 불러오기
> git clone https://github.com/EOSIO/eosio.contracts.git

### 2. 터미널(1) 첫번째 keosd 실행
> keosd

### 3. 새 터미널(2) 기본 월렛 만들기
> cleos wallet create --to-console

### 4. 패스워드 저장

### 5. 키 생성
> cleos create key

### 6. 프라이빗 키 등록
> cleos wallet import --private-key [프라이빗키]

### 7. 새 터미널(3)에서 첫번째 노드 시작
> nodeos --enable-stale-production --producer-name eosio --plugin eosio::chain_api_plugin --plugin eosio::net_api_plugin

### 8. 새 터미널(4)에서 eosio.bios 진입
> cd eosio.contracts/contracts/eosio.bios
> 
> mkdir build
> 
> cd build
> 
> mkdir eosio.bios
> 
> eosio.bios build
> 
> cd eosio.contracts/contracts/eosio.bios/src
> 
> eosio-cpp -I ../include -abigen -o eosio.bios.wasm eosio.bios.cpp

### 9. eosio.bios 이동
> mv eosio.bios.wasm ../build/eosio.bios
> 
> mv eosio.bios.abi ../build/eosio.bios

### 10. 터미널(2)에서 set contract
> cd eosio.contracts/contracts/eosio.bios/build
> 
> cleos set contract [계정이름] [배포경로] [.wasm 파일 경로] [.abi 파일 경로]
> 
> cleos set contract eosio eosio.bios --abi eosio.bios.abi

### 11. 계정 생성
> EOSIO Key 임포트
> 
> Public : EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
> 
> Private : 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

### 12. 키 생성 -> 키 임포트
> cleos create account eosio [계정명] [퍼블릭키] [퍼블릭키]
>
> cleos 키 임포트 추가

### 13. 터미널(4)에서 노드 실행
> nodeos --enable-stale-production --producer-name [계정명] --plugin eosio::chain_api_plugin --plugin eosio::net_api_plugin --http-server-address 127.0.0.1:8888 --p2p-listen-endpoint 127.0.0.1:9877 --p2p-peer-address 127.0.0.1:9876 --config-dir node2 --data-dir node2 --signature-provider = [퍼블릭키]=KEY:[프라이빗키]

### 14. 노드 실패 확인

### 15. push action
> cleos push action eosio setprods "{ "schedule": [{"producer_name": "[계정명]","block_signing_key": "[]"}]}" -p eosio@active

### 16. 노드 확인

### 17. 심화
> https://developers.eos.io/eosio-nodeos/docs/local-multi-node-testnet
