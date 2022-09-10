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

  const MyToken = await ethers.getContractFactory("MyToken");
  const erc20TokenContract = await MyToken.deploy("10000000000000000000000000000000000");
  await erc20TokenContract.deployed();

  const [owner, ...accounts] = await ethers.getSigners();

  let firstTime = 0;
  for (let i = 0; i < 10; ++i) {
    firstTime += (await erc20TokenContract.estimateGas.transfer(accounts[i].address, 100000)).toNumber();
    await erc20TokenContract.transfer(accounts[i].address, 100000);
  }

  console.log("Tranfer token to account not hold token: ", firstTime/10);
  let secondTime = 0;
  for (let i = 0; i < 10; ++i) {
    secondTime += (await erc20TokenContract.estimateGas.transfer(accounts[i].address, 100000)).toNumber();
    await erc20TokenContract.transfer(accounts[i].address, 100000);
  }

  console.log("Transfer token to account holded token", secondTime/10);
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });
