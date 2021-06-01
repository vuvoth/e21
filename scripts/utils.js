const toBigNumber = require("ethers").BigNumber.from;
const utils = require("ethers").utils;
const fs = require("fs");
const _ = require("lodash");
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


module.exports= {
  readProof, 
  createTxInputs
}
