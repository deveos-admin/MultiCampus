const Eos = require('eosjs')
const config={
    httpEndpoint: 'https://jungle2.cryptolions.io:443'
}

Eos(config).getInfo((error,info)=>{
    if(error){
        console.error(error);
    }
    console.log(info);
});
