// We require the Hardhat Runtime Environment explicitly here. This is optional
// but useful for running the script in a standalone fashion through `node <script>`.
//
// When running the script with `hardhat run <script>` you'll find the Hardhat
// Runtime Environment's members available in the global scope.
const hre = require("hardhat");
const { ethers } = require("hardhat");
const fs = require("fs");
const block = require("../data/block.json");

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

  console.log((await erc20TokenContract.balanceOf(zkRollupContract.address)).toString())

  let numberAccount = await zkRollupContract.numberAccount();

  console.log(block["number_account"])
  for (let i = 1; i < block["number_account"]; ++i) {
    let ethAccount = await zkRollupContract.EthAddressOf(i);
    console.log(`${ethAccount}  = ${block["leaf"][i]["balance"]}`);
  }
  return true;
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });
