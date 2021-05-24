# e21

Implement zk-rollup use libsnark and node js for my thesis. 

Thank community for good idea about layer 2 and zk-rollup.

## Architecture

## Data schemas 

### Account

```json
{
  acountId: 20bits
  balance: 32bits; 
  nonce: 32bits;
  pubKey: (px, py) // EdDSA publickey 
}
```

map(accountETH => accountRollup)


### Transition 

```
accountId_from || accountId_to || amount
```

## Rollup Operation 

### Deposist 

#### Input Data


```json 
{
  "addressETH": 20 bytes Ethereum account
  "amount": 32 bit
}
```



