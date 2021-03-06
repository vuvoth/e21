const { expect } = require("chai");
const toBigNumber = require("ethers").BigNumber.from;
const utils = require("ethers").utils;
const fs = require("fs");
const _ = require("lodash");
const { ethers } = require("hardhat");
const NUMBER_TX_PRE_BATCH = 60;
const MERKLE_DEEP = 8;
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

describe.skip("Verifier testing", function () {
  let vk, proof;
  let state;
  let MyToken, Main, erc20Token;
  let accounts, owner, instance;
  before(async function () {
    vk = JSON.parse(fs.readFileSync("./verify_key.txt").toString());
    proof = JSON.parse(fs.readFileSync("./witness_transaction.json").toString());
    state = JSON.parse(fs.readFileSync("./roll_up_transaction.json").toString());

    Main = await ethers.getContractFactory("Main");
    MyToken = await ethers.getContractFactory("MyToken");
    erc20Token = await MyToken.deploy(1000000);
    instance = await Main.deploy(
      proof.input[proof.input.length - 1],
      erc20Token.address
    );
    [owner, ...accounts] = await ethers.getSigners();

    for (let account of accounts) {
      await erc20Token.transfer(account.address, 1000);
      //console.log(`Creat 1000 token for account ${account.address}`);
      await erc20Token.connect(account).approve(instance.address, 1000);
      await instance.connect(account).resigter(1, 2, 1000);
    }
  });


  it("Should veirfy success", async function () {
    let inVk = _.flattenDeep([vk.alpha, vk.beta, vk.gamma, vk.delta]);
    let vkGammaABC = _.flattenDeep(vk.gammaABC);
    let inProof = _.flattenDeep([proof.A, proof.B, proof.C]);
    let input = proof.input;
    let transaction_inputs = createTxInputs(state);

    console.log("Rollup and withdraw");

    let result = await instance.zkRollup(
      inVk,
      vkGammaABC,
      inProof,
      transaction_inputs,
      input[3],
      input[0]
    );

  });
});
