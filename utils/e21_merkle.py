import sys
import random
from ethsnarks.merkletree import MerkleTree
from ethsnarks.field import FQ
from ethsnarks.mimc import *
from ethsnarks.eddsa import *
from ethsnarks.jubjub import Point

from copy import copy, deepcopy
import json
TREE_DEEP = 10
AMOUNT_SIZE = 20

class Account(object):
    def __init__(self, public_key, balance, nonce):
        self.public_key = public_key
        self.balance = balance
        self.nonce = nonce
    
    def update_balance(self, balance):
        self.balance = balance

    def get_details(self):
        return [self.public_key.x, self.public_key.y, self.balance, self.nonce]
    
    def sendAsset(self, amount):
        self.nonce = self.nonce + 1 
        self.balance = self.balance - amount
    
    def receiveAsset(self, amount):
        self.balance = self.balance + amount
    def getBalance(self):
        return self.balance
    def toDict(self):
        return {
                "public_key_x": str(self.public_key.x),
                "public_key_y": str(self.public_key.y),
                "balance": str(self.balance), 
                "nonce": str(self.nonce)
                }

class ZkRollup(object):
    def __init__(self):
        # only use for test. we don't store private data off users in product
        self.client_data = dict() 
        self.account_number = 0
        self.state = dict()
        self.merkle_tree = MerkleTree(1<<TREE_DEEP)
        self.hasher = mimc_hash;
        self.signature = PureEdDSA()

    def hash_leaf(self, account): 
        return self.hasher(map(int, account.get_details()), 1)

    def add_account(self, account):
        self.merkle_tree.append(self.hash_leaf(account))
    
    def get_account_by_index(self, index):
        return self.state[index] 

    def create_account(self): 
        k, public_key = EdDSA.random_keypair()
        #k = FQ(self.account_number + 1)

        #public_key = self.signature.B() * k
        account = Account(public_key, 0, 0)
        self.add_account(account);
        self.state[self.account_number] = account;
        self.client_data[self.account_number] = k
        self.account_number = self.account_number + 1

    def update(self, index, account):
        self.state[index] = account
        self.merkle_tree.update(index, self.hash_leaf(account))
    
    def set_account_balance(self, index, balance):
        account = self.get_account_by_index(index)
        account.update_balance(balance)
        self.update(index, account)

    def get_proof(self, index):
        proof = self.merkle_tree.proof(index)
        return {
                "hash_proof": list(map(str,proof.path)), 
                "address": str(index),
                 "hash_leaf": str(proof.leaf)
                }
    
    def generate_transaction_proof(self,tx_sign, sender_id, receiver_id, tx_amount):
        sender = self.get_account_by_index(sender_id)
        receiver = self.get_account_by_index(receiver_id)
        tx_data = {
                "sender_id": str(sender_id), 
                "receiver_id": str(receiver_id),
                "sender": sender.toDict(),
               "receiver": receiver.toDict(),
               "amount": str(tx_amount), 
               "proof_sender": self.get_proof(sender_id),
               "proof_receiver": self.get_proof(receiver_id)
        }
        return json.dumps(tx_data)

    def tranfer_asset(self, sender_id, receiver_id, amount):    

        sender = self.get_account_by_index(sender_id)
        receiver = self.get_account_by_index(receiver_id)
        
        tx_proof = {
                "sender_proof": self.get_proof(sender_id),
                "sender": sender.toDict(), 
        }

        tx_proof["sender_proof"]["merkle_root"] = str(FQ(self.merkle_tree.root));

        raw_m = [FQ(sender_id, 1<<TREE_DEEP), FQ(receiver_id, 1 << TREE_DEEP), FQ(amount, 1 << AMOUNT_SIZE), FQ(sender.nonce, 1 << TREE_DEEP)] 
        sig_m = self.signature.to_bits(*raw_m)
        tx_sign = self.signature.sign(sig_m, self.client_data[sender_id])

        tx_proof['signature'] = {
                "R_x": str(tx_sign.sig.R.x),
                "R_y": str(tx_sign.sig.R.y),
                "s": str(tx_sign.sig.s)
                }

        tx_proof["amount"]= str(amount)        

        sender.sendAsset(amount)
        self.update(sender_id, sender)

        tx_proof["middle_root"] = str(self.merkle_tree.root);
        
        tx_proof["receiver_proof"] = self.get_proof(receiver_id)
        tx_proof["receiver"] = receiver.toDict();

        receiver.receiveAsset(amount)
        self.update(receiver_id, receiver)

        return tx_proof
        

def main():
    args = sys.argv[1:]
    total_account = int(args[0])
    number_tx = int(args[1])
    out_file_path = args[2]    

    tx_proof = {
            "number_tx": number_tx
            }
    zkr = ZkRollup();
    for i in range(total_account):     
        zkr.create_account();
        zkr.set_account_balance(i, FQ(1000, 1 << AMOUNT_SIZE))

    for i in range(number_tx):    
        while True:
          sender_id = random.randrange(0, total_account)
          receiver_id = random.randrange(0, total_account)
          if(sender_id != receiver_id): break;

        sender = zkr.get_account_by_index(sender_id)
        amount = random.randrange(1, sender.balance)
        
        tx_proof["tx" + str(i)] = zkr.tranfer_asset(sender_id, receiver_id, FQ(amount, 1 << AMOUNT_SIZE ))
        
        print(sender_id, zkr.get_account_by_index(sender_id).get_details())
        print(receiver_id,zkr.get_account_by_index(receiver_id).get_details())

    with open(out_file_path, "w") as out_file: 
        json.dump(tx_proof, out_file, indent=4)

main()

