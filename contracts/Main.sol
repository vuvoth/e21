pragma solidity ^0.5.0;

import "../libs/ethsnarks/contracts/Verifier.sol";
import "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract Main {
  uint256 constant public MERKLE_DEEP = 16; 
  uint256 constant public AMOUNT_SIZE = 32;
  uint256 constant public NUMBER_TX_PRE_BATCH = 60;
  uint256 constant public NUMBER_INPUT = NUMBER_TX_PRE_BATCH * 4 + 2;
    
  struct LeafData{
    uint256 pubkey_x; 
    uint256 pubkey_y; 
    uint256 amount; 
    uint256 nonce;
  }
    

  uint256 public numberAccount;
  bytes32 public zkroot;  
  address public token_address;
  mapping(address => uint) public  zkAddressOf; 
  mapping(uint => address) public EthAddressOf;
  mapping(uint256 => LeafData) public zk_data;

  constructor(bytes32 _zkroot, address _token_address) public {
    zkroot = _zkroot;
    token_address = _token_address;
  }

  function setZkroot(uint256 _zkroot) internal {
    zkroot = bytes32(_zkroot);
  }


  function sliceBytes(bytes memory bytes_, uint256 begin_, uint256 end_) internal pure returns (bytes memory) {
        bytes memory slice = new bytes(end_ - begin_);
        for (uint index = 0; index + begin_ < end_; index = index + 1) {
          slice[index] = bytes_[index  + begin_];
        }
        return slice;
  }
  
  function splitToTwo128(bytes32 y) public pure returns(bytes16, bytes16) {
    return (bytes16(y << 128),bytes16((y >> 128) << 128));
  }
  
  function toLibsnarkBits(bytes16 y) public pure returns(bytes16){
    uint128 k = 0;
    uint128 itow = 1 << 127;
    uint128 tmp = uint128(y);
    for (uint i = 0; i < 128; ++i) {
      k = k + uint128(tmp & 1) * itow;
      itow = itow >> 1;
      tmp = tmp >> 1;
    }
    return bytes16(k);
  }
  function toUint16(bytes memory bs, uint start) internal pure returns (uint16 x) {
    require(bs.length >= start + 2, "slicing out of range");
    assembly {
        x := mload(add(bs, add(0x2, start)))
    }
    return x;
  }
  
  function toUint32(bytes memory bs, uint start) internal pure returns (uint32 x) {
    require(bs.length >= start + 4, "slicing out of range");
    assembly {
        x := mload(add(bs, add(0x4, start)))
    }
    return x;
  }
  function transaction_input_content(bytes memory transaction_input, uint256 begin_) public pure 
  returns(
    uint16 sender_id, 
    uint16 receiver_id, 
    uint32 amount, 
    uint16 nonce
  )
  {
    sender_id = toUint16(transaction_input, begin_) ;
    receiver_id = toUint16(transaction_input, begin_ + 2);
    amount = toUint32(transaction_input, begin_ + 4);
    nonce = toUint16(transaction_input, begin_ + 8); 
  } 

  // tranfer token in, out layer 
  function zkIO(bytes memory transaction_inputs) internal returns(bool) {
    uint transaction_size = (MERKLE_DEEP * 3 + AMOUNT_SIZE) / 8;
    for (uint index = 0; index < transaction_inputs.length; index = index + transaction_size ) {
      (
        uint16 sender_id, 
        uint16 receiver_id, 
        uint32 amount, 
        uint16 nonce
      ) = transaction_input_content(transaction_inputs, index);
      
      if (sender_id == 0) {
        // move out layer 2
        IERC20(token_address).transfer(EthAddressOf[receiver_id], amount);
      } 
      else if (receiver_id == 0) {
        // move in player 2
        if (EthAddressOf[sender_id] != address(0))
          IERC20(token_address).transferFrom(EthAddressOf[sender_id], address(this), amount);
      } 
    }
    return true;
  }
  

  function create_account(address eth_address, uint256 pubkey_x, uint256 pubkey_y, uint amount) public {
    numberAccount++; 
    zk_data[numberAccount].pubkey_x = pubkey_x; 
    zk_data[numberAccount].pubkey_y = pubkey_y; 
    zk_data[numberAccount].amount = amount;
    EthAddressOf[numberAccount] = eth_address;
    zkAddressOf[eth_address] = numberAccount;
  }

  function resigter(uint256 pubkey_x, uint256 pubkey_y, uint amount) public {
    require(zkAddressOf[msg.sender] == 0, "you created acccount before");
    IERC20(token_address).transferFrom(msg.sender, address(this), amount);
    create_account(msg.sender, pubkey_x, pubkey_y, amount);
  }
  
  function zkRollup(
    uint256[14] memory in_vk, 
    uint256[] memory vk_gammaABC, 
    uint256[8] memory in_proof,
    bytes memory transaction_inputs,
    uint256 old_root, 
    uint256 new_root
  ) public returns(bool) {
    
    uint256[] memory proof_inputs = new uint256[](4);
  
    proof_inputs[0] = new_root;

    bytes32 input_hash = sha256(transaction_inputs);

    (bytes16 second, bytes16 first) = splitToTwo128(input_hash); 
    
    second = toLibsnarkBits(second);
    first = toLibsnarkBits(first);
  
    proof_inputs[1] = uint128(first); 
    proof_inputs[2] = uint128(second);
    
    proof_inputs[3] = old_root;

    require(zkroot == bytes32(proof_inputs[3]), "current_root not match");

    require(Verifier.Verify(in_vk, vk_gammaABC, in_proof, proof_inputs), "verify failed");

    zkIO(transaction_inputs);    

    setZkroot(new_root);

    return true;
  }
  
  function to_transaction_input_bytes(uint16 sender_id, uint16 receiver_id, uint32 amount, uint16 nonce) internal returns(bytes memory) {
    return abi.encodePacked(bytes2(sender_id), bytes2(receiver_id), bytes4(amount), bytes2(nonce));  
  }
  

  function depositToken(
    uint256[14] memory in_vk, 
    uint256[] memory vk_gammaABC, 
    uint256[8] memory in_proof,
    uint16 account_id, 
    uint256 old_root, 
    uint256 new_root
  ) public returns(bool) {
    bytes memory transaction_inputs = to_transaction_input_bytes(account_id, 0, 0, 0);

    zkRollup(in_vk, vk_gammaABC, in_proof, transaction_inputs, old_root, new_root);  
    
    //IERC20(token_address).transferFrom();  
  }

  function withdrawToken(
    uint256[14] memory in_vk, 
    uint256[] memory vk_gammaABC, 
    uint256[8] memory in_proof,
    uint16 account_id, uint32 amount, uint16 nonce,
    uint256 old_root, 
    uint256 new_root
  ) public returns(bool) {
    bytes memory transaction_inputs = to_transaction_input_bytes(account_id, 0, amount, nonce);
    zkRollup(in_vk, vk_gammaABC, in_proof, transaction_inputs, old_root, new_root);  
    //IERC20(token_address).transfer(EthAddressOf[account_id]);    
  }
}


