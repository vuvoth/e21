// We require the Hardhat Runtime Environment explicitly here. This is optional
// but useful for running the script in a standalone fashion through `node <script>`.
//
// When running the script with `hardhat run <script>` you'll find the Hardhat
// Runtime Environment's members available in the global scope.
const hre = require("hardhat");
const { ethers } = require("hardhat");
const fs = require("fs");
const utils = require("./utils.js");
const block = require('../data/block.json');
async function loadContractAddress(path) {
  return JSON.parse(fs.readFileSync(path));
}
async function main() {
  const { tokenAddress, zkrollAddress } = await loadContractAddress(
    "./data/contract_address.json"
  );

  console.log(tokenAddress, zkrollAddress);
  let {commit_block} = await JSON.parse(fs.readFileSync("./data/commit_block.txt"))
  console.log(commit_block)
  const MyToken = await ethers.getContractFactory("MyToken");
  const erc20TokenContract = await MyToken.attach(tokenAddress);

  const zkRollup = await ethers.getContractFactory("Main");
  const zkRollupContract = await zkRollup.attach(zkrollAddress);

  let proof = utils.readProof(
    "./data/vk.txt",
    `./data/${commit_block}_witness.json`,
    `./data/${commit_block}_transaction.json`
  );
  let transaction_inputs = utils.createTxInputs(proof.data);


  
  let gas = await zkRollupContract.estimateGas.zkRollup(
    ...proof.params,
    transaction_inputs,
    proof.public_input[3],
    proof.public_input[0]
  );
  console.log("Gas code for rollup: " + gas.toString())
  
  console.log("Send transaction to netwok!")
  let result = await zkRollupContract.zkRollup(
    ...proof.params,
    transaction_inputs,
    proof.public_input[3],
    proof.public_input[0]
  );
  commit_block = commit_block + 1;
  let obj = {commit_block};
  await fs.writeFileSync("./data/commit_block.txt", JSON.stringify(obj));

  return 0;
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });
