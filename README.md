# e21

Implement zk-rollup use libsnark and node js for my thesis. 

Thank community for good idea about layer 2 and zk-rollup.

## Build and start 

### Build

```bash
git submodule update --init --recursive
npm install
./script.sh config 
./script.sh build
```

### Run 

```bash
export tx_per_epoch=10
export number_user=10 # should less than 20 because hardhat only have 20 default accounts.
./zkrle21 init $number_user $tx_per_epoch
./zkrle21 deposit $tx_per_epoch
./zkrle21 # query balance of account
./zkrle21 transfer $tx_per_epoch 
./zkrle21 withdraw $tx_per_epoch
```

## Architecture
## Docs

inprocess.......

## References

- [ethsnark](https://github.com/HarryR/ethsnarks)
- [roll_up](https://github.com/barryWhiteHat/roll_up)
- [An Incomplete Guide to Rollups](https://vitalik.ca/general/2021/01/05/rollup.html) by Vitalik


