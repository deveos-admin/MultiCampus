import React, { Component } from 'react';
import './App.css';
import Eos from 'eosjs';

const config = {

  chainId: 'e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473', // 32 byte (64 char) hex string
  keyProvider: ['5KSfurDTM5M8YxipJfh4CBYAPDTSkXk3wKoqWCXdusQpcz4sQK4', '5J7EoqiTwdwBAZ1RV5nk6fqVjFDvRr6CaA2pRJiJ5SK6bj9gdzb'],
  httpEndpoint: 'https://jungle2.cryptolions.io:443',
  expireInSeconds: 60,
  broadcast: true,
  verbose: true, // API activity
  sign: true
}

const eosjs = Eos(config);

const contract = 'dlwodnjs1111';
const myaccount = 'doraemonydwy'


class App extends Component {
  state = {
    ball: 0,
    result: ''
  }

  ballChange = (e) => {
    this.setState({
      ball: e.target.value
    })
  }
  resultChange = (e) => {
    this.setState({
      result: e.target.value
    })
  }

  StartFunction = () => {
    eosjs.transaction(
      {
        actions:
          [
            {
              account: contract,
              name: 'start',
              authorization: [{
                actor: myaccount,
                permission: 'active'
              }],
              data: {
                player: myaccount
              }
            }
          ]
      }
    ).then(result => {
      console.log(result)
    })
  }

  ThrowBallFunction = () => {
    eosjs.transaction(
      {
        actions:
          [
            {
              account: contract,
              name: 'throwball',
              authorization: [{
                actor: myaccount,
                permission: 'active'
              }],
              data: {
                player: myaccount,
                value: this.state.ball
              }
            }
          ]
      }
    ).then(result => {
      console.log(result);
    })
  }

  ResultFunction = () => {
    eosjs.getTableRows({
      code: contract,
      scope: myaccount,
      table: 'status',
      lower_bound: myaccount,
      limit: 1,
      json: true
    }).then(result => {
      this.setState({ result: result.rows[0].text });
    })
  }


  render() {
    return (
      <div className="BaseBallGame">
        <div>
          <ul><button onClick={this.StartFunction}>Start</button></ul>
          <ul>
            <form id="form1">
              <input type="text" id="ball" value={this.state.ball} onChange={this.ballChange} />
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
