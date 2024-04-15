/*******************************************************************************
*  (c) 2019 Zondax GmbH
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

#include "parser_impl.h"
#include "cbor.h"
#include <cbor/cbor_parser_helper.h>

parser_tx_t parser_tx_obj;

const char *parser_getErrorDescription(parser_error_t err) {
    switch (err) {
        case parser_ok:
            return "No error";
        case parser_no_data:
            return "No more data";
        case parser_init_context_empty:
            return "Initialized empty context";
        case parser_unexpected_buffer_end:
            return "Unexpected buffer end";
        case parser_unexpected_version:
            return "Unexpected version";
        case parser_unexpected_characters:
            return "Unexpected characters";
        case parser_unexpected_field:
            return "Unexpected field";
        case parser_duplicated_field:
            return "Unexpected duplicated field";
        case parser_value_out_of_range:
            return "Value out of range";
        case parser_unexpected_chain:
            return "Unexpected chain";
        case parser_query_no_results:
            return "item query returned no results";
        case parser_missing_field:
            return "missing field";
        case parser_unexpected_type:
            return "unexpected type";
//////
        case parser_display_idx_out_of_range:
            return "display index out of range";
        case parser_display_page_out_of_range:
            return "display page out of range";
//////
        case parser_json_zero_tokens:
            return "JSON. Zero tokens";
        case parser_json_too_many_tokens:
            return "JSON. Too many tokens";
        case parser_json_incomplete_json:
            return "JSON string is not complete";
        case parser_json_contains_whitespace:
            return "JSON Contains whitespace in the corpus";
        case parser_json_is_not_sorted:
            return "JSON Dictionaries are not sorted";
        case parser_json_missing_chain_id:
            return "JSON Missing chain_id";
        case parser_json_missing_sequence:
            return "JSON Missing sequence";
        case parser_json_missing_fee:
            return "JSON Missing fee";
        case parser_json_missing_msgs:
            return "JSON Missing msgs";
        case parser_json_missing_account_number:
            return "JSON Missing account number";
        case parser_json_missing_memo:
            return "JSON Missing memo";
        case parser_json_unexpected_error:
            return "JSON Unexpected error";
        // cbor
        case parser_cbor_unexpected:
            return "unexpected CBOR error";
        case parser_cbor_not_canonical:
            return "CBOR was not in canonical order";
        case parser_cbor_unexpected_EOF:
            return "Unexpected CBOR EOF";
        // Context specific
        case parser_context_mismatch:
            return "context prefix is invalid";
        case parser_context_unexpected_size:
            return "context unexpected size";
        case parser_context_invalid_chars:
            return "context invalid chars";
        case parser_transaction_too_big:
            return "Transaction is too big";

        default:
            return "Unrecognized error code";
    }
}

parser_error_t _read_json_tx(parser_context_t *c, __Z_UNUSED parser_tx_t *v) {
    parser_error_t err = json_parse(&parser_tx_obj.tx_json.json,
                                    (const char *) c->buffer,
                                    c->bufferLen);
    if (err != parser_ok) {
        return err;
    }

    parser_tx_obj.tx_json.tx = (const char *) c->buffer;
    parser_tx_obj.tx_json.flags.cache_valid = 0;
    parser_tx_obj.tx_json.filter_msg_type_count = 0;
    parser_tx_obj.tx_json.filter_msg_from_count = 0;

    return parser_ok;
}