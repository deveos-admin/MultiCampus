import React, { Component } from 'react';
import './App.css';
import Eos from 'eosjs';
import ScatterJS from 'scatterjs-core';
import ScatterEOS from 'scatterjs-plugin-eosjs';


ScatterJS.plugins(new ScatterEOS());


const network = {
  blockchain: 'eos',
  protocol: 'https',
  host: 'jungle2.cryptolions.io',                //메인
  port: 443,
  chainId: 'e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473'
}

 



class App extends Component {
  state ={
    ball:0,
    result:''
  }

  ballChange =(e)=>{
    this.setState({
      ball:e.target.value
    })
  }
  resultChange =(e)=>{
    this.setState({
      result:e.target.value
    })
  }

  StartFunction=()=>
{
  ScatterJS.scatter.connect('My-App').then(connected=>{
    if (!connected) return false;
    var scatter = ScatterJS.scatter;
    var requiredFields = { accounts: [network] };
    scatter.getIdentity(requiredFields).then(()=>{
      var account = scatter.identity.accounts.find(x => x.blockchain === 'eos');
      var eosOptions = { expireInSeconds: 60 };
      var eos = scatter.eos(network, Eos, eosOptions);
      eos.transaction(
        {
          actions: 
          [
            {
              account: 'dlwodnjs1111',
              name: 'start',
              authorization: [{
                actor: account.name,
                permission: account.authority
              }],
              data: {
                player: account.name
              }
            }
          ]
        }
      ).then(result=>{
        console.log(result)
      })

    })

  })

}

ThrowBallFunction=()=>
{

  ScatterJS.scatter.connect('My-App').then(connected=>{
    if (!connected) return false;
    var scatter = ScatterJS.scatter;
    var requiredFields = { accounts: [network] };
    scatter.getIdentity(requiredFields).then(()=>{
      var account = scatter.identity.accounts.find(x => x.blockchain === 'eos');
      var eosOptions = { expireInSeconds: 60 };
      var eos = scatter.eos(network, Eos, eosOptions);
      eos.transaction(
        {
          actions: 
          [
            {
              account: 'dlwodnjs1111',
              name: 'throwball',
              authorization: [{
                actor: account.name,
                permission: account.authority
              }],
              data: {
                player: account.name,
                value: this.state.ball
              }
            }
          ]
        }
      ).then(result=>{
        console.log(result);
      })
    })

  })



}

ResultFunction=()=>
{
  ScatterJS.scatter.connect('My-App').then(connected=>{
    if (!connected) return false;
    var scatter = ScatterJS.scatter;
    var requiredFields = { accounts: [network] };
    scatter.getIdentity(requiredFields).then(()=>{
      var account = scatter.identity.accounts.find(x => x.blockchain === 'eos');
      var eosOptions = { expireInSeconds: 60 };
      var eos = scatter.eos(network, Eos, eosOptions);
      eos.getTableRows({
        code:'dlwodnjs1111',
        scope:account.name,
        table: 'status',
        lower_bound: account.name,
        limit: 1,
        json: true
      }).then(result=>{
        this.setState({result:result.rows[0].text});
      })
    
    })

  })


  
}


  render(){
    return (
      <div className="App">
        <div>
          <ul><button onClick={this.StartFunction}>Start</button></ul>
          <ul>
            <form id="form1">
              <input type="text" id="ball" value={this.state.ball} onChange={this.ballChange}/>
            </form>
            <button onClick={this.ThrowBallFunction} >ThrowBall</button>
          </ul>
          <ul>
            <button onClick={this.ResultFunction} >ResultCheck</button>
          </ul>
          <ul>
          {this.state.result}
          </ul>
        </div>
        <div>
          {this.state.ball}
        </div>

      </div>
    );
  }
  
}



export default App;
