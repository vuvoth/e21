const {expect} = require("chai");

const fs = require("fs");
const _ = require("lodash");

describe("Verifier testing", function() {
  let vk, proof;
  before(() => {
    vk = JSON.parse(fs.readFileSync("./verifier_key.txt").toString());
    proof = JSON.parse(fs.readFileSync("./winess.txt").toString());
  })

  it("Should veirfy success", async function() {
    const Main = await ethers.getContractFactory("Main");

    let inVk = _.flattenDeep([ vk.alpha, vk.beta, vk.gamma, vk.delta ]);
    let vkGammaABC = _.flattenDeep(vk.gammaABC);
    let inProof = _.flattenDeep([ proof.A, proof.B, proof.C ]);
    let input = proof.input;

    const instance = await Main.deploy(proof.input[input.length - 1]);

    await instance.deployed();

    await instance.verifyTx(inVk, vkGammaABC, inProof, input);
  });
});
