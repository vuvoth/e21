const { expect } = require("chai");
const toBigNumber = require("ethers").BigNumber.from;
const utils = require("ethers").utils;
const fs = require("fs");
const _ = require("lodash");
const { ethers } = require("hardhat");
const NUMBER_TX_PRE_BATCH = 60;
const MERKLE_DEEP = 16;
const AMOUNT_SIZE = 32;

function createTxInputs(state) {
  let transaction_inputs = "0x";
  for (let i = 0; i < state["number_tx"]; ++i) {
    let sender_address = utils.hexlify(
      toBigNumber(state["tx" + i]["sender_proof"]["address"])
    );
    let receiver_address = utils.hexlify(
      toBigNumber(state["tx" + i]["receiver_proof"]["address"])
    );
    let amount = utils.hexlify(toBigNumber(state["tx" + i]["amount"]));
    let nonce = utils.hexlify(toBigNumber(state["tx" + i]["sender"]["nonce"]));

    sender_address = utils.hexZeroPad(sender_address, MERKLE_DEEP / 8);
    receiver_address = utils.hexZeroPad(receiver_address, MERKLE_DEEP / 8);
    amount = utils.hexZeroPad(amount, AMOUNT_SIZE / 8);
    nonce = utils.hexZeroPad(nonce, MERKLE_DEEP / 8);

    transaction_inputs +=
      sender_address.slice(2) +
      receiver_address.slice(2) +
      amount.slice(2) +
      nonce.slice(2);
  }
  return transaction_inputs;
}


function readProof(vk_path, witness_path, data_path) {
  vk = JSON.parse(fs.readFileSync(vk_path).toString());
  proof = JSON.parse(fs.readFileSync(witness_path).toString());
  state = JSON.parse(fs.readFileSync(data_path).toString());
  let inVk = _.flattenDeep([vk.alpha, vk.beta, vk.gamma, vk.delta]);
  let vkGammaABC = _.flattenDeep(vk.gammaABC);
  let inProof = _.flattenDeep([proof.A, proof.B, proof.C]);
  let input = proof.input;

  return {
    params: [inVk, vkGammaABC, inProof], 
    public_input: input,
    data: state
  }
}

describe("Transaction and withdraw", function () {
  let vk, proof;
  let state;
  let MyToken, Main, erc20Token;
  let accounts, owner, instance;
  before(async function () {

    let proof = readProof("./verify_key.txt", "./witness_transaction.json", "roll_up_transaction.json");

    Main = await ethers.getContractFactory("Main");
    MyToken = await ethers.getContractFactory("MyToken");
    erc20Token = await MyToken.deploy(1000000);
    instance = await Main.deploy(
      proof.public_input[3],
      erc20Token.address
    );

    [owner, ...accounts] = await ethers.getSigners();
    console.log("       Init token......")
    for (let account of accounts) {
      await erc20Token.transfer(account.address, 1000);
      await erc20Token.connect(account).approve(instance.address, 1000);
      await instance.connect(account).resigter(1, 2, 1000);
    }
  });


  it("Transfer token inside layer 2", async function () {
    let proof = readProof("./verify_key.txt", "./witness_transaction.json", "roll_up_transaction.json");
    let transaction_inputs = createTxInputs(proof.data);


    console.log("Rollup transactions");

    let result = await instance.zkRollup(
      ...proof.params,
      transaction_inputs,
      proof.public_input[3],
      proof.public_input[0]
    );

  });

  it("Withdraw token", async() => {

    let proof = readProof("./verify_key.txt", "./witness_withdraw.json", "./roll_up_withdraw.json");
    let transaction_inputs = createTxInputs(proof.data);

    console.log("withdraw");

    let result = await instance.zkRollup(
      ...proof.params,
      transaction_inputs,
      proof.public_input[3],
      proof.public_input[0]
    );

    let total = toBigNumber(0);
    for (let account of accounts) {
      let amount = await erc20Token.balanceOf(account.address);
      total = total.add(amount);
      console.log(`Number token off  account ${account.address} = ${await erc20Token.balanceOf(account.address)}`);
    }
    console.log(total.toString());
  })
});
