import {expect, test} from "jest";
import Zemu from "@zondax/zemu";
import CosmosApp from "ledger-cosmos-js";
import secp256k1 from "secp256k1/elliptic";
import crypto from "crypto";

const Resolve = require("path").resolve;
const APP_PATH = Resolve("../app/bin/app.elf");

const APP_SEED = "equip will roof matter pink blind book anxiety banner elbow sun young"
const sim_options = {
    logging: true,
    start_delay: 4000,
    custom: `-s "${APP_SEED}"`
    , X11: true
};

jest.setTimeout(30000)

const example_tx_str_MsgSend = {
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
};

const example_tx_str_MsgDeposit = {
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
};

describe('Basic checks', function () {
    it('can start and stop container', async function () {
        const sim = new Zemu(APP_PATH);
        try {
            await sim.start(sim_options);
        } finally {
            await sim.close();
        }
    });

    it('get app version', async function () {
        const sim = new Zemu(APP_PATH);
        try {
            await sim.start(sim_options);
            const app = new CosmosApp(sim.getTransport());
            const resp = await app.getVersion();

            console.log(resp);

            expect(resp.return_code).toEqual(0x9000);
            expect(resp.error_message).toEqual("No errors");
            expect(resp).toHaveProperty("test_mode");
            expect(resp).toHaveProperty("major");
            expect(resp).toHaveProperty("minor");
            expect(resp).toHaveProperty("patch");
        } finally {
            await sim.close();
        }
    });

    it('get app info', async function () {
        const sim = new Zemu(APP_PATH);
        try {
            await sim.start(sim_options);
            const app = new CosmosApp(sim.getTransport());
            const info = await app.appInfo();

            console.log(info)
        } finally {
            await sim.close();
        }
    });

    // NOTE: Temporarily Disabled due to Ledger's Request
    // it('get device info', async function () {
    //     const sim = new Zemu(APP_PATH);
    //     try {
    //         await sim.start(sim_options);
    //         const app = new CosmosApp(sim.getTransport());
    //         const resp = await app.deviceInfo();
    //
    //         console.log(resp);
    //
    //         expect(resp.return_code).toEqual(0x9000);
    //         expect(resp.error_message).toEqual("No errors");
    //
    //         expect(resp).toHaveProperty("targetId");
    //         expect(resp).toHaveProperty("seVersion");
    //         expect(resp).toHaveProperty("flag");
    //         expect(resp).toHaveProperty("mcuVersion");
    //     } finally {
    //         await sim.close();
    //     }
    // });

    it('get address', async function () {
        const sim = new Zemu(APP_PATH);
        try {
            await sim.start(sim_options);
            const app = new CosmosApp(sim.getTransport());
            const path = [44, 931, 0, 0, 0];
            const resp = await app.getAddressAndPubKey(path, "thor");

            console.log(resp);

            expect(resp.return_code).toEqual(0x9000);
            expect(resp.error_message).toEqual("No errors");

            expect(resp).toHaveProperty("bech32_address");
            expect(resp).toHaveProperty("compressed_pk");
            
            // Independently derived from Ian Colman BIP39
            // https://github.com/iancoleman/bip39/pull/492
            expect(resp.bech32_address).toEqual("thor1mwyrp6lj85swy5e5g4hjlaacm33g6rw3p4qmq4");
            expect(resp.compressed_pk.length).toEqual(33);
        } finally {
            await sim.close();
        }
    });

    it('sign MsgSend', async function () {

        const sim = new Zemu(APP_PATH);
        try {
            await sim.start(sim_options);
            const app = new CosmosApp(sim.getTransport());

            const path = [44, 931, 0, 0, 0];
            let tx = JSON.stringify(example_tx_str_MsgSend);

            // get address / publickey
            const respPk = await app.getAddressAndPubKey(path, "thor");
            expect(respPk.return_code).toEqual(0x9000);
            expect(respPk.error_message).toEqual("No errors");
            console.log(respPk)

            // do not wait here..
            const signatureRequest = app.sign(path, tx);

            await Zemu.sleep(3000);

            // Reference window
            for (let i = 0; i < 8; i++) {
                await sim.clickRight();
            }
            await sim.clickBoth();

            let resp = await signatureRequest;
            console.log(resp);

            expect(resp.return_code).toEqual(0x9000);
            expect(resp.error_message).toEqual("No errors");

            // Now verify the signature
            const hash = crypto.createHash("sha256");
            const msgHash = Uint8Array.from(hash.update(tx).digest());

            const signatureDER = resp.signature;
            const signature = secp256k1.signatureImport(Uint8Array.from(signatureDER));

            const pk = Uint8Array.from(respPk.compressed_pk)

            const signatureOk = secp256k1.ecdsaVerify(signature, msgHash, pk);
            expect(signatureOk).toEqual(true);

        } finally {
            await sim.close();
        }
    });

    it('sign MsgDeposit', async function () {

        const sim = new Zemu(APP_PATH);
        try {
            await sim.start(sim_options);
            const app = new CosmosApp(sim.getTransport());

            const path = [44, 931, 0, 0, 0];
            let tx = JSON.stringify(example_tx_str_MsgDeposit);

            // get address / publickey
            const respPk = await app.getAddressAndPubKey(path, "thor");
            expect(respPk.return_code).toEqual(0x9000);
            expect(respPk.error_message).toEqual("No errors");
            console.log(respPk)

            // do not wait here..
            const signatureRequest = app.sign(path, tx);

            await Zemu.sleep(3000);

            // Reference window
            for (let i = 0; i < 7; i++) {
                await sim.clickRight();
            }
            await sim.clickBoth();

            let resp = await signatureRequest;
            console.log(resp);

            expect(resp.return_code).toEqual(0x9000);
            expect(resp.error_message).toEqual("No errors");

            // Now verify the signature
            const hash = crypto.createHash("sha256");
            const msgHash = Uint8Array.from(hash.update(tx).digest());

            const signatureDER = resp.signature;
            const signature = secp256k1.signatureImport(Uint8Array.from(signatureDER));

            const pk = Uint8Array.from(respPk.compressed_pk)

            const signatureOk = secp256k1.ecdsaVerify(signature, msgHash, pk);
            expect(signatureOk).toEqual(true);

        } finally {
            await sim.close();
        }
    });


    it('sign expert MsgSend', async function () {

        const sim = new Zemu(APP_PATH);
        try {
            await sim.start(sim_options);
            const app = new CosmosApp(sim.getTransport());

            const path = [44, 931, 0, 0, 0];
            let tx = JSON.stringify(example_tx_str_MsgSend);

            // get address / publickey
            const respPk = await app.getAddressAndPubKey(path, "thor");
            expect(respPk.return_code).toEqual(0x9000);
            expect(respPk.error_message).toEqual("No errors");
            console.log(respPk)

            // Switch to Expert Mode
            await sim.clickRight();
            await sim.clickRight();
            await sim.clickBoth();

            // do not wait here..
            const signatureRequest = app.sign(path, tx);

            await Zemu.sleep(3000);

            // Reference window
            for (let i = 0; i < 11; i++) {
                await sim.clickRight();
            }
            await sim.clickBoth();

            let resp = await signatureRequest;
            console.log(resp);

            expect(resp.return_code).toEqual(0x9000);
            expect(resp.error_message).toEqual("No errors");

            // Now verify the signature
            const hash = crypto.createHash("sha256");
            const msgHash = Uint8Array.from(hash.update(tx).digest());

            const signatureDER = resp.signature;
            const signature = secp256k1.signatureImport(Uint8Array.from(signatureDER));

            const pk = Uint8Array.from(respPk.compressed_pk)

            const signatureOk = secp256k1.ecdsaVerify(signature, msgHash, pk);
            expect(signatureOk).toEqual(true);

        } finally {
            await sim.close();
        }
    });
});
