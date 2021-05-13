/*******************************************************************************
*   (c) 2018 Zondax GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "gtest/gtest.h"
#include <json/json_parser.h>
#include <tx_validate.h>
#include <common/parser.h>
#include "util/common.h"

namespace {
    TEST(TxValidationTest, CorrectFormat) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_ok) << "Validation failed, error: " << parser_getErrorDescription(err);
    }
    TEST(TxValidationTest, MissingAccountNumber) {
        auto transaction =
            R"({"chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);

        EXPECT_EQ(err, parser_json_missing_account_number) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, MissingChainId) {
        auto transaction =
            R"({"account_number":"588","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_missing_chain_id) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, MissingFee) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_missing_fee) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, MissingMsgs) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_missing_msgs) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, MissingSequence) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}]})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_missing_sequence) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, Spaces_InTheMiddle) {
        auto transaction =
            R"({"account_number":"588", "chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_contains_whitespace) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, Spaces_AtTheFront) {
        auto transaction =
            R"({  "account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_contains_whitespace) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, Spaces_AtTheEnd) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"  })";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_contains_whitespace) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, Spaces_Lots) {
        auto transaction =
            R"({"account_number": "588","chain_id":"thorchain"  ,"fee":{"amount": [ ],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_contains_whitespace) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, AllowSpacesInString) {
        auto transaction =
            R"({"account_number":"588","chain_id":"  thorchain  ","fee":{"amount":[],"gas":"2000000"},"memo":"  TestMemo  ","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_ok) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, SortedDictionary) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_ok) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, NotSortedDictionary_FirstElement) {
        auto transaction =
            R"({"chain_id":"thorchain","account_number":"588","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_is_not_sorted) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, NotSortedDictionary_MiddleElement) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","fee":{"gas":"2000000","amount":[]},"memo":"TestMemo","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}],"sequence":"5"})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_is_not_sorted) << "Validation failed, error: " << parser_getErrorDescription(err);
    }

    TEST(TxValidationTest, NotSortedDictionary_LastElement) {
        auto transaction =
            R"({"account_number":"588","chain_id":"thorchain","fee":{"amount":[],"gas":"2000000"},"memo":"TestMemo","sequence":"5","msgs":[{"type":"thorchain/MsgSend","value":{"amount":[{"amount":"150000000","denom":"rune"}],"from_address":"tthor1c648xgpter9xffhmcqvs7lzd7hxh0prgv5t5gp","to_address":"tthor10xgrknu44d83qr4s4uw56cqxg0hsev5e68lc9z"}}]})";

        parsed_json_t json;
        parser_error_t err;

        err = JSON_PARSE(&json, transaction);
        ASSERT_EQ(err, parser_ok);

        err = tx_validate(&json);
        EXPECT_EQ(err, parser_json_is_not_sorted) << "Validation failed, error: " << parser_getErrorDescription(err);
    }
}
