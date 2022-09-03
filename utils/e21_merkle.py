import sys
import random
from ethsnarks.merkletree import MerkleTree
from ethsnarks.field import FQ
from ethsnarks.mimc import *
from ethsnarks.eddsa import *
from ethsnarks.jubjub import Point

from copy import copy, deepcopy
import json
TREE_DEEP = 8
AMOUNT_SIZE = 32
TOKEN_SUPPLY = int(1e9)
class Account(object):
    def __init__(self, public_key, balance, nonce):
        self.public_key = public_key
        self.balance = balance
        self.nonce = nonce
    
    def update_balance(self, balance):
        self.balance = self.balance + balance 
    def set_balance(self, balance): 
        self.balance = balance;

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
        self.block = 0

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
    
    def update_account_balance(self, index, balance):
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
    def write_data(self, file_path): 
        data = {}
        leaf_data = {}
        for index in self.state:
            account_data = self.state[index].toDict()
            account_data["k"] = str(self.client_data[index].n)
            leaf_data[index] = account_data
        data["block"] = self.block + 1
        data["root"] = str(self.merkle_tree.root)
        data["leaf"] = leaf_data
        data["number_account"] = self.account_number;
        with open(file_path, "w") as out_file: 
            json.dump(data, out_file, indent=4)

    def read_data(self, file_path): 
        with open(file_path, "r") as read_file:
            data = json.load(read_file)
        self.block = data['block'];
        leaf_data = data['leaf']
        self.account_number = 0;
        for leaf_index in leaf_data:
            leaf = leaf_data[leaf_index]
            public_key = Point(FQ(int(leaf["public_key_x"])), 
                        FQ(int(leaf["public_key_y"])))
            account = Account(public_key, int(leaf['balance']), int(leaf['nonce'])) 
            self.add_account(account)
            self.client_data[int(leaf_index)] = FQ(int(leaf['k']))
            self.account_number = self.account_number  + 1 
            self.state[int(leaf_index)] = account
    
    def gen_internal_transaction(self) : 
        sender_id = 0;
        receiver_id = 0;
        while True:
            sender_id = random.randrange(1, self.account_number)
            receiver_id = random.randrange(1, self.account_number)
            sender = self.get_account_by_index(sender_id) 
            if(sender_id != receiver_id) and (sender.balance != 0): 
                if (sender.balance == FQ(1, 1 << AMOUNT_SIZE)):
                    amount = FQ(1, 1 << AMOUNT_SIZE)
                else:
                    amount = random.randrange(1, int(sender.balance))
                return [sender_id, receiver_id, amount]

    def gen_withdraw(self, account_id) : 
        sender_id = account_id;
        receiver_id = 0;

        sender = self.get_account_by_index(sender_id) 
        amount = sender.balance
        return [sender_id, receiver_id, amount]
    
    def gen_deposit(self, account_id, amount = 100): 
        sender_id = 0;
        receiver_id = account_id;
        return [sender_id, receiver_id, amount]

    def mechanism(self, sign, index): 
        if sign == "internal":
            transaction = self.gen_internal_transaction()
        elif sign == "withdraw":
            transaction = self.gen_withdraw(index)
        elif sign == "deposit":
            transaction = self.gen_deposit(index)
        return transaction

def init_data(total_account): 
    zkr = ZkRollup();
    print("Create account.....")
    for i in range(0, total_account):     
        if (i  + 1) % 10 == 0:
            print("Creating and init data from user %d to %d..." %((i - 9), i))
        zkr.create_account();
    zkr.update_account_balance(0, FQ(TOKEN_SUPPLY,1 << AMOUNT_SIZE))
    return zkr

def create_transactions(zkr, number_tx, mechanism_type,  out_file_path):
    tx_proof = {
            "number_tx": number_tx 
            }
    
    total_account = zkr.account_number
    print("Create transaction.......")


    for i in range(number_tx):    
        sender_id, receiver_id, amount = zkr.mechanism(mechanism_type, (i) % (total_account - 1) + 1)
        print("%d => %d = %d" %(sender_id, receiver_id, amount))
        tx_proof["tx" + str(i)] = zkr.tranfer_asset(sender_id, receiver_id, FQ(amount, 1 << AMOUNT_SIZE )) 

    tx_proof["final_merkle_root"] = str(zkr.merkle_tree.root)
    tx_proof["block"] = str(zkr.block);
    with open(out_file_path, "w") as out_file: 
        json.dump(tx_proof, out_file, indent=4)

def main(): 
    args = sys.argv[1:]
    option = args[0]
    if option == "init":
        number_account = int(args[1])
        zkr = init_data(number_account);
        zkr.write_data("./data/block.json")
    else:
        number_tx = int(args[1])
        zkr = ZkRollup();
        zkr.read_data("./data/block.json")
        out_file = "./data/" + str(zkr.block) + "_transaction.json"
        if option == "gen_transfer":
           create_transactions(zkr, number_tx, "internal", out_file);
           zkr.write_data("./data/block.json")
        elif option == "gen_withdraw": 
           create_transactions(zkr, number_tx, "withdraw", out_file);
           zkr.write_data("./data/block.json")
        elif option == "gen_deposit": 
           create_transactions(zkr, number_tx, "deposit", out_file);
           zkr.write_data("./data/block.json")

main()
