pragma solidity ^0.5.0;

import "../libs/ethsnarks/contracts/Verifier.sol";


contract Main {
    function verifyTx(
        uint256[14] memory in_vk, 
        uint256[] memory vk_gammaABC, 
        uint256[8] memory in_proof, 
        uint256[] memory proof_inputs
    ) public view returns(bool) {
        return Verifier.Verify(in_vk, vk_gammaABC, in_proof, proof_inputs);
    }
}


