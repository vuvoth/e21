from ethsnarks.merkletree import MerkleTree
from ethsnarks.field import FQ
from ethsnarks.mimc import *
from ethsnarks.eddsa import *
from ethsnarks.jubjub import Point

from copy import copy, deepcopy
import json
TREE_DEEP = 2


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
        account = Account(public_key, FQ(0), FQ(0))
        self.add_account(account);
        self.state[self.account_number] = account;
        self.client_data[self.account_number] = k
        self.account_number = self.account_number + 1

    def update(self, index, account):
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
                "receiver_proof": self.get_proof(receiver_id), 
                "sender": sender.toDict(), 
                "receiver": receiver.toDict(),
        }

        tx_proof["sender_proof"]["merkle_root"] = str(FQ(self.merkle_tree.root));

        sender.sendAsset(amount)
        receiver.receiveAsset(amount)

        raw_m = [FQ(sender_id), FQ(receiver_id), FQ(amount), FQ(sender.nonce)] 

        m_sig = self.signature.to_bytes(*raw_m)
        
        tx_sign = self.signature.sign(m_sig, self.client_data[sender_id])
        
        tx_proof['signature'] = {
                "R.x": str(tx_sign.sig.R.x),
                "R.y": str(tx_sign.sig.R.y),
                "s": str(tx_sign.sig.s)
                }
        tx_proof["amount"]= str(amount)        

        self.update(sender_id, sender)

        tx_proof["middle_root"] = str(self.merkle_tree.root);

        self.update(receiver_id, receiver)


        print(json.dumps(tx_proof))
        return tx_proof
        
zkm_tree = ZkRollup()

zkm_tree.create_account();
zkm_tree.create_account();

zkm_tree.set_account_balance(0, FQ(1000))

zkm_tree.tranfer_asset(0, 1, FQ(10))
#zkm_tree.tranfer_asset(1, 0, FQ(8))
