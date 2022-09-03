// We require the Hardhat Runtime Environment explicitly here. This is optional
// but useful for running the script in a standalone fashion through `node <script>`.
//
// When running the script with `hardhat run <script>` you'll find the Hardhat
// Runtime Environment's members available in the global scope.
const hre = require("hardhat");
const fs = require("fs");
const block = require('../data/block.json')
async function main() {
  // Hardhat always runs the compile task when running scripts with its command
  // line interface.
  //
  // If this script is run directly using `node` you may want to call compile
  // manually to make sure everything is compiled
  // await hre.run('compile');

  // We get the contract to deploy
  
  console.log("Deploying Token");
  const MyToken = await hre.ethers.getContractFactory("MyToken");
  const erc20Contract = await MyToken.deploy("10000000000000000000000000000000000");

  await erc20Contract.deployed();

  console.log("Token deployed to:", erc20Contract.address);
  console.log("---------------------------\n");
  console.log("Deploying zkRollup");
  
  const zkRollup = await hre.ethers.getContractFactory("Main");

  const root = ethers.BigNumber.from(block.root)
  const zkRollupContract = await zkRollup.deploy(root, erc20Contract.address);
  await zkRollupContract.deployed();

  console.log("ZkRollup deployed to: ", zkRollupContract.address);

  let addresses = { tokenAddress: erc20Contract.address, zkrollAddress : zkRollupContract.address};

  await fs.writeFileSync("./data/contract_address.json", JSON.stringify(addresses));
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main()
  .then(() => process.exit(0))
  .catch((error) => {
    console.error(error);
    process.exit(1);
  });
