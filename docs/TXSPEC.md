Transaction Specification
-------------------------

### Format

Transactions passed to the Ledger device will be in the following format. The Ledger device MUST accept any transaction (valid as below) in this format.

```json
{
  "account_number": {number},
  "chain_id": {string},
  "fee": {
    "amount": [],
    "gas": {number}
  },
  "memo": {string},
  "msgs": [{arbitrary}],
  "sequence": {number}
}
```

`msgs` is a list of messages, which are arbitrary JSON structures. Ledger app currently supports `MsgDeposit` & `MsgSend` (examples below).

#### Examples

```json
{
  "account_number": "588",
  "chain_id": "thorchain",
  "fee": {
    "amount": [],
    "gas": "2000000"
  },
  "memo": "TestMemo",
  "msgs": [
    {
      "type": "thorchain/MsgSend",
      "value": {
        "amount": [
          {
            "amount": "150000000",
            "denom": "rune"
          }
        ],
        "from_address": "tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp",
        "to_address": "tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"
      }
    }
  ],
  "sequence": "5"
}
```

```json
{
  "account_number": "588",
  "chain_id": "thorchain",
  "fee": {
    "amount": [],
    "gas": "10000000"
  },
  "memo": "",
  "msgs": [
    {
      "type": "thorchain/MsgDeposit",
      "value": {
        "coins": [
          {
            "amount": "330000000",
            "asset": "THOR.RUNE"
          }
        ],
        "memo": "SWAP:BNB.BNB:tbnb1qk2m905ypazwfau9cn0qnr4c4yxz63v9u9md20:",
        "signer": "tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp"
      }
    }
  ],
  "sequence": "6"
}
```

#### Display Logic

The Ledger device SHOULD pick a suitable display representation for the transaction.

The key type (secp256k1 / ed25519), `account_number`, `chain_id`, `fee` , `memo`, and `sequence`, should be displayed in that order, each on their own page, autoscrolling if necessary. Some fields may be clipped when not in advanced mode.

`msgs` should be iterated through and each displayed according to the following recursive logic:

```
display (json, level)
  if level == 2
    show value as json-encoded string
  else
    switch typeof(json) {
      case object:
        for (key, value) in object:
          show key
          display(value, level + 1)
      case array:
        for element in array:
          display(element, level + 1)
      otherwise:
        show value as json-encoded string
    }
```

starting at level 0, e.g. `display(msgs[0], 0)`.

### Validation

The Ledger device MUST validate that supplied JSON is valid. Our JSON specification is a subset of [RFC 7159](https://tools.ietf.org/html/rfc7159) - invalid RFC 7159 JSON is invalid Ledger JSON, but not all valid RFC 7159 JSON is valid Ledger JSON.

We add the following two rules:
- No spaces or newlines can be included, other than escaped in strings
- All dictionaries must be serialized in lexicographical key order

This serves to prevent signature compatibility issues among different client libraries.

This is equivalent to the following Python snippet:

```python
import json

def ledger_validate(json_str):
  obj = json.loads(json_str)
  canonical = json.dumps(obj, sort_keys = True, separators = (',', ':'))
  return canonical == json_str

assert ledger_validate('{"a":2,"b":3}')
assert ledger_validate('{"a ":2,"b":3}')
assert not ledger_validate('{"a":2,\n"b":3}')
assert not ledger_validate('{"b":2,"a":3}')
assert not ledger_validate('{"a" : 2 }')
```