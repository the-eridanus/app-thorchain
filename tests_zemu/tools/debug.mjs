import Zemu from "@zondax/zemu";
import CosmosApp from "ledger-cosmos-js";
import path from "path";

const APP_PATH = path.resolve(`./../../app/bin/app.elf`);

const seed = "equip will roof matter pink blind book anxiety banner elbow sun young"
const SIM_OPTIONS = {
    logging: true,
    start_delay: 4000,
    X11: true,
    custom: `-s "${seed}" --color LAGOON_BLUE --sdk 2.0`
};

// MsgSend
const example_MsgSend_tx_str = {"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"};

// MsgDeposit
const example_MsgDeposit_tx_str = {"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"10000000"},"memo":"","msgs":[{"type":"thorchain/MsgDeposit","value":{"coins":[{"amount":"330000000","asset":"THOR.RUNE"}],"memo":"SWAP:BNB.BNB:tbnb1qk2m905ypazwfau9cn0qnr4c4yxz63v9u9md20:","signer":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp"}}],"sequence":"6"};

async function beforeStart() {
    process.on("SIGINT", () => {
        Zemu.default.stopAllEmuContainers(function () {
            process.exit();
        });
    });
    await Zemu.default.checkAndPullImage();
}

async function beforeEnd() {
    await Zemu.default.stopAllEmuContainers();
}

async function debugScenario(sim, app) {
    const path = [44, 931, 0, 0, 0];

    let tx = JSON.stringify(example_MsgDeposit_tx_str);
    //let tx = JSON.stringify(example_MsgSend_tx_str);


//    await Zemu.default.sleep(120000);
    //const addr = await app.getAddressAndPubKey(path, "tthor");
    //console.log(addr)

    //console.log(tx);

    // do not wait here..
    const signatureRequest = app.sign(path, tx);
    //await Zemu.default.sleep(100000);

    // await sim.clickRight();
    // await sim.clickRight();
    // await sim.clickRight();
    // await sim.clickRight();
    // await sim.clickRight();
    // await sim.clickRight();
    // await sim.clickRight();
    // await sim.clickRight();
    // await sim.clickBoth();

    let resp = await signatureRequest;
    console.log(resp);
}

async function main() {
    await beforeStart();

    if (process.argv.length > 2 && process.argv[2] === "debug") {
        SIM_OPTIONS["custom"] = SIM_OPTIONS["custom"] + " --debug";
    }

    const sim = new Zemu.default(APP_PATH);

    try {
        await sim.start(SIM_OPTIONS);
        const app = new CosmosApp.default(sim.getTransport());

        ////////////
        /// TIP you can use zemu commands here to take the app to the point where you trigger a breakpoint

        await debugScenario(sim, app);

        /// TIP

    } finally {
        await sim.close();
        await beforeEnd();
    }
}

(async () => {
    await main();
})();
