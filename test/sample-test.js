const { expect } = require("chai");

const fs = require("fs");
const _ = require("lodash");

describe("Verifier testing", function() {
  it("Should return the new greeting once it's changed", async function() {
    const Main = await ethers.getContractFactory("Main");
    const instance = await Main.deploy();
    
    await instance.deployed();
    let vk = JSON.parse(fs.readFileSync("./build/vk.txt").toString());
    let proof = JSON.parse(fs.readFileSync("./build/proof.txt").toString());

    let inVk = _.flattenDeep([vk.alpha, vk.beta, vk.gamma, vk.delta]);
    let vkGammaABC = _.flattenDeep(vk.gammaABC);
    let inProof = _.flattenDeep([proof.A, proof.B, proof.C]);
    let input = [];

   /* console.log({
        inVk, vkGammaABC, inProof,input
    })*/

    expect(await instance.verifyTx(inVk, vkGammaABC, inProof, input)).to.equal(true);
  });
});
