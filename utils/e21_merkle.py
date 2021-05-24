from ethsnarks.merkletree import MerkleTree
from ethsnarks.mimc import *

TREE_DEEP = 2



class AccountZkR(object):
    def __init__(self, public_key, balance, nonce):
        self.public_key = public_key
        self.balance = balance
        self.nonce = nonce
    
    def get_details(self):
        return [self.public_key.x, self.public_key.y, balance, nonce]

class ZkRollup(object):
    def __init__(self):
        self.merkle_tree = MerkleTree(1<<TREE_DEEP)
    def add_account(self, account):
        self.merkle_tree.append(account.get_details())
    
