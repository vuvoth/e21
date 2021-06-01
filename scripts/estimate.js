// We require the Hardhat Runtime Environment explicitly here. This is optional
// but useful for running the script in a standalone fashion through `node <script>`.
//
// When running the script with `hardhat run <script>` you'll find the Hardhat
// Runtime Environment's members available in the global scope.
const hre = require("hardhat");
const { ethers } = require("hardhat");
const fs = require("fs");
const utils = require("./utils.js");
const block = require('../data/block.json')
async function loadContractAddress(path) {
  return JSON.parse(fs.readFileSync(path));
}
async function main() {
  const { tokenAddress, zkrollAddress } = await loadContractAddress(
    "./data/contract_address.json"
  );
  

  console.log(tokenAddress, zkrollAddress);
  const MyToken = await ethers.getContractFactory("MyToken");
  const erc20TokenContract = await MyToken.attach(tokenAddress);

  const zkRollup = await ethers.getContractFactory("Main");
  const zkRollupContract = await zkRollup.attach(zkrollAddress);
  const [owner, ...accounts] = await ethers.getSigners();

  console.log("gas code for simple transfer:")
  let gas = await erc20TokenContract.estimateGas.transfer(accounts[0].address, 1000);
  console.log(gas.toString());
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
