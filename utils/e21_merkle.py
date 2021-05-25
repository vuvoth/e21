from ethsnarks.merkletree import MerkleTree
from ethsnarks.field import FQ
from ethsnarks.mimc import *
from ethsnarks.eddsa import EdDSA
from ethsnarks.jubjub import Point

from copy import copy, deepcopy

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

class ZkRollup(object):
    def __init__(self):
        self.merkle_tree = MerkleTree(1<<TREE_DEEP)

    def add_account(self, account):
        self.merkle_tree.append(mimc_hash(map(int, account.get_details()), 1))
    
    def update(self, index, account):
        self.merkle_tree.update(index,mimc_hash(map(int, account.get_details()), 1))

    def get_proof(self, index):
        print(self.merkle_tree.root)
        return self.merkle_tree.proof(index)



#k, public_key = EdDSA.random_keypair()

x = FQ(10979212538355656650585917615931941520607504799522162730049831890951746946604);
y = FQ(5324824554426725410290295966356728817595938893815438489688542305809226945840);
public_key = Point(x, y);

balance = FQ(2000);
nonce = FQ(1000);
amount= FQ(10);

account_state = Account(public_key, balance, nonce)

accoun_new = deepcopy(account_state)
accoun_new.update_balance(FQ(1990))
accoun_new.nonce = FQ(1001)
zkm_tree = ZkRollup();


zkm_tree.add_account(account_state)


print(zkm_tree.get_proof(0))


# account_state.balance = FQ(990);
print(account_state.balance);
zkm_tree.update(0, accoun_new)

print(zkm_tree.get_proof(0))

print(account_state.get_details(), accoun_new.get_details())
