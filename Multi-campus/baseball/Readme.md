1. 정글넷 세팅

2. 정글넷 계정 생성

3. sudo 권한으로 진입

4. vim .bashrc

5. allias jcleos='/usr/opt/eosio/1.7.0/bin/cleos -u https://jungle2.cryptolions.io:443'

6. source .bashrc

7. jcleos get info

8. mkdir baseball 

9. vim baseball.cpp 후 :wq 로 파일 생성

10. vim baseball.hpp 후, :wq로 파일 생성 (VS CODE를 이용하여 윈도우 환경에서 파일을 생성하면 권한 충돌이 발생할 수 있음)

11. 폴더명, 파일명이 같아야 build 시 좀 더 편함

12. 소스코드 작성 - 코드 내 contract명은 파일명과 같아야함. 그렇지 같으면 빌드해도 action 및 table이 등록되지 않음.

13. eosio-cpp -o baseball.wasm baseball.cpp -abigen 로 빌드. -o 옵션은 output의 약자로 baseball.cpp 파일로 baseball.wasm 결과물을 생성하겠다라는 뜻. -abigen 옵션으로 baseball.abi 생성. 

14. cleos set contract 계정명 경로 명령어로 배포 (ex. cleos set contract devtooth ../baseball)

15. 폴더 경로

- C:\Users\[student]\AppData\Local\Packages\CanonicalGroupLimited.Ubuntu18.04onWindows_79rhkp1fndgsc\LocalState\rootfs\home\soolmini
