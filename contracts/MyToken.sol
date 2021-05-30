pragma solidity ^0.5.0;

import "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import "@openzeppelin/contracts/token/ERC20/ERC20Detailed.sol";

/*
 * Create Erc20 token from openzepplin contract. Thank @openzeppelin for amazing work....
 */
contract MyToken is ERC20, ERC20Detailed {
    constructor(uint256 initialSupply) ERC20Detailed("My Token", "MT", 0) public {
        _mint(msg.sender, initialSupply);
    }
}
