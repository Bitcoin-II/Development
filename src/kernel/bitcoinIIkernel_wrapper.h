// Copyright (c) 2024-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOINII_KERNEL_BITCOINIIKERNEL_WRAPPER_H
#define BITCOINII_KERNEL_BITCOINIIKERNEL_WRAPPER_H

#include <kernel/bitcoinIIkernel.h>

#include <array>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace bc2k {

enum class LogCategory : bc2k_LogCategory {
    ALL = bc2k_LogCategory_ALL,
    BENCH = bc2k_LogCategory_BENCH,
    BLOCKSTORAGE = bc2k_LogCategory_BLOCKSTORAGE,
    COINDB = bc2k_LogCategory_COINDB,
    LEVELDB = bc2k_LogCategory_LEVELDB,
    MEMPOOL = bc2k_LogCategory_MEMPOOL,
    PRUNE = bc2k_LogCategory_PRUNE,
    RAND = bc2k_LogCategory_RAND,
    REINDEX = bc2k_LogCategory_REINDEX,
    VALIDATION = bc2k_LogCategory_VALIDATION,
    KERNEL = bc2k_LogCategory_KERNEL
};

enum class LogLevel : bc2k_LogLevel {
    TRACE_LEVEL = bc2k_LogLevel_TRACE,
    DEBUG_LEVEL = bc2k_LogLevel_DEBUG,
    INFO_LEVEL = bc2k_LogLevel_INFO
};

enum class ChainType : bc2k_ChainType {
    MAINNET = bc2k_ChainType_MAINNET,
    TESTNET = bc2k_ChainType_TESTNET,
    TESTNET_4 = bc2k_ChainType_TESTNET_4,
    SIGNET = bc2k_ChainType_SIGNET,
    REGTEST = bc2k_ChainType_REGTEST
};

enum class SynchronizationState : bc2k_SynchronizationState {
    INIT_REINDEX = bc2k_SynchronizationState_INIT_REINDEX,
    INIT_DOWNLOAD = bc2k_SynchronizationState_INIT_DOWNLOAD,
    POST_INIT = bc2k_SynchronizationState_POST_INIT
};

enum class Warning : bc2k_Warning {
    UNKNOWN_NEW_RULES_ACTIVATED = bc2k_Warning_UNKNOWN_NEW_RULES_ACTIVATED,
    LARGE_WORK_INVALID_CHAIN = bc2k_Warning_LARGE_WORK_INVALID_CHAIN
};

enum class ValidationMode : bc2k_ValidationMode {
    VALID = bc2k_ValidationMode_VALID,
    INVALID = bc2k_ValidationMode_INVALID,
    INTERNAL_ERROR = bc2k_ValidationMode_INTERNAL_ERROR
};

enum class BlockValidationResult : bc2k_BlockValidationResult {
    UNSET = bc2k_BlockValidationResult_UNSET,
    CONSENSUS = bc2k_BlockValidationResult_CONSENSUS,
    CACHED_INVALID = bc2k_BlockValidationResult_CACHED_INVALID,
    INVALID_HEADER = bc2k_BlockValidationResult_INVALID_HEADER,
    MUTATED = bc2k_BlockValidationResult_MUTATED,
    MISSING_PREV = bc2k_BlockValidationResult_MISSING_PREV,
    INVALID_PREV = bc2k_BlockValidationResult_INVALID_PREV,
    TIME_FUTURE = bc2k_BlockValidationResult_TIME_FUTURE,
    HEADER_LOW_WORK = bc2k_BlockValidationResult_HEADER_LOW_WORK
};

enum class ScriptVerifyStatus : bc2k_ScriptVerifyStatus {
    OK = bc2k_ScriptVerifyStatus_OK,
    ERROR_INVALID_FLAGS_COMBINATION = bc2k_ScriptVerifyStatus_ERROR_INVALID_FLAGS_COMBINATION,
    ERROR_SPENT_OUTPUTS_REQUIRED = bc2k_ScriptVerifyStatus_ERROR_SPENT_OUTPUTS_REQUIRED,
};

enum class ScriptVerificationFlags : bc2k_ScriptVerificationFlags {
    NONE = bc2k_ScriptVerificationFlags_NONE,
    P2SH = bc2k_ScriptVerificationFlags_P2SH,
    DERSIG = bc2k_ScriptVerificationFlags_DERSIG,
    NULLDUMMY = bc2k_ScriptVerificationFlags_NULLDUMMY,
    CHECKLOCKTIMEVERIFY = bc2k_ScriptVerificationFlags_CHECKLOCKTIMEVERIFY,
    CHECKSEQUENCEVERIFY = bc2k_ScriptVerificationFlags_CHECKSEQUENCEVERIFY,
    WITNESS = bc2k_ScriptVerificationFlags_WITNESS,
    TAPROOT = bc2k_ScriptVerificationFlags_TAPROOT,
    ALL = bc2k_ScriptVerificationFlags_ALL
};

template <typename T>
struct is_bitmask_enum : std::false_type {
};

template <>
struct is_bitmask_enum<ScriptVerificationFlags> : std::true_type {
};

template <typename T>
concept BitmaskEnum = is_bitmask_enum<T>::value;

template <BitmaskEnum T>
constexpr T operator|(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<std::underlying_type_t<T>>(lhs) | static_cast<std::underlying_type_t<T>>(rhs));
}

template <BitmaskEnum T>
constexpr T operator&(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<std::underlying_type_t<T>>(lhs) & static_cast<std::underlying_type_t<T>>(rhs));
}

template <BitmaskEnum T>
constexpr T operator^(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<std::underlying_type_t<T>>(lhs) ^ static_cast<std::underlying_type_t<T>>(rhs));
}

template <BitmaskEnum T>
constexpr T operator~(T value)
{
    return static_cast<T>(~static_cast<std::underlying_type_t<T>>(value));
}

template <BitmaskEnum T>
constexpr T& operator|=(T& lhs, T rhs)
{
    return lhs = lhs | rhs;
}

template <BitmaskEnum T>
constexpr T& operator&=(T& lhs, T rhs)
{
    return lhs = lhs & rhs;
}

template <BitmaskEnum T>
constexpr T& operator^=(T& lhs, T rhs)
{
    return lhs = lhs ^ rhs;
}

template <typename T>
T check(T ptr)
{
    if (ptr == nullptr) {
        throw std::runtime_error("failed to instantiate bc2k object");
    }
    return ptr;
}

template <typename Collection, typename ValueType>
class Iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using iterator_concept = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = ValueType;

private:
    const Collection* m_collection;
    size_t m_idx;

public:
    Iterator() = default;
    Iterator(const Collection* ptr) : m_collection{ptr}, m_idx{0} {}
    Iterator(const Collection* ptr, size_t idx) : m_collection{ptr}, m_idx{idx} {}

    // This is just a view, so return a copy.
    auto operator*() const { return (*m_collection)[m_idx]; }
    auto operator->() const { return (*m_collection)[m_idx]; }

    auto& operator++() { m_idx++; return *this; }
    auto operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

    auto& operator--() { m_idx--; return *this; }
    auto operator--(int) { auto temp = *this; --m_idx; return temp; }

    auto& operator+=(difference_type n) { m_idx += n; return *this; }
    auto& operator-=(difference_type n) { m_idx -= n; return *this; }

    auto operator+(difference_type n) const { return Iterator(m_collection, m_idx + n); }
    auto operator-(difference_type n) const { return Iterator(m_collection, m_idx - n); }

    auto operator-(const Iterator& other) const { return static_cast<difference_type>(m_idx) - static_cast<difference_type>(other.m_idx); }

    ValueType operator[](difference_type n) const { return (*m_collection)[m_idx + n]; }

    auto operator<=>(const Iterator& other) const { return m_idx <=> other.m_idx; }

    bool operator==(const Iterator& other) const { return m_collection == other.m_collection && m_idx == other.m_idx; }

private:
    friend Iterator operator+(difference_type n, const Iterator& it) { return it + n; }
};

template <typename Container, typename SizeFunc, typename GetFunc>
concept IndexedContainer = requires(const Container& c, SizeFunc size_func, GetFunc get_func, std::size_t i) {
    { std::invoke(size_func, c) } -> std::convertible_to<std::size_t>;
    { std::invoke(get_func, c, i) }; // Return type is deduced
};

template <typename Container, auto SizeFunc, auto GetFunc>
    requires IndexedContainer<Container, decltype(SizeFunc), decltype(GetFunc)>
class Range
{
public:
    using value_type = std::invoke_result_t<decltype(GetFunc), const Container&, size_t>;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator<Range, value_type>;
    using const_iterator = iterator;

private:
    const Container* m_container;

public:
    explicit Range(const Container& container) : m_container(&container)
    {
        static_assert(std::ranges::random_access_range<Range>);
    }

    iterator begin() const { return iterator(this, 0); }
    iterator end() const { return iterator(this, size()); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    size_t size() const { return std::invoke(SizeFunc, *m_container); }

    bool empty() const { return size() == 0; }

    value_type operator[](size_t index) const { return std::invoke(GetFunc, *m_container, index); }

    value_type at(size_t index) const
    {
        if (index >= size()) {
            throw std::out_of_range("Index out of range");
        }
        return (*this)[index];
    }

    value_type front() const { return (*this)[0]; }
    value_type back() const { return (*this)[size() - 1]; }
};

#define MAKE_RANGE_METHOD(method_name, ContainerType, SizeFunc, GetFunc, container_expr) \
    auto method_name() const & { \
        return Range<ContainerType, SizeFunc, GetFunc>{container_expr}; \
    } \
    auto method_name() const && = delete;

template <typename T>
std::vector<std::byte> write_bytes(const T* object, int (*to_bytes)(const T*, bc2k_WriteBytes, void*))
{
    std::vector<std::byte> bytes;
    struct UserData {
        std::vector<std::byte>* bytes;
        std::exception_ptr exception;
    };
    UserData user_data = UserData{.bytes = &bytes, .exception = nullptr};

    constexpr auto const write = +[](const void* buffer, size_t len, void* user_data) -> int {
        auto& data = *reinterpret_cast<UserData*>(user_data);
        auto& bytes = *data.bytes;
        try {
            auto const* first = static_cast<const std::byte*>(buffer);
            auto const* last = first + len;
            bytes.insert(bytes.end(), first, last);
            return 0;
        } catch (...) {
            data.exception = std::current_exception();
            return -1;
        }
    };

    if (to_bytes(object, write, &user_data) != 0) {
        std::rethrow_exception(user_data.exception);
    }
    return bytes;
}

template <typename CType>
class View
{
protected:
    const CType* m_ptr;

public:
    explicit View(const CType* ptr) : m_ptr{check(ptr)} {}

    const CType* get() const { return m_ptr; }
};

template <typename CType, CType* (*CopyFunc)(const CType*), void (*DestroyFunc)(CType*)>
class Handle
{
protected:
    CType* m_ptr;

public:
    explicit Handle(CType* ptr) : m_ptr{check(ptr)} {}

    // Copy constructors
    Handle(const Handle& other)
        : m_ptr{check(CopyFunc(other.m_ptr))} {}
    Handle& operator=(const Handle& other)
    {
        if (this != &other) {
            Handle temp(other);
            std::swap(m_ptr, temp.m_ptr);
        }
        return *this;
    }

    // Move constructors
    Handle(Handle&& other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
    Handle& operator=(Handle&& other) noexcept
    {
        DestroyFunc(m_ptr);
        m_ptr = std::exchange(other.m_ptr, nullptr);
        return *this;
    }

    template <typename ViewType>
        requires std::derived_from<ViewType, View<CType>>
    Handle(const ViewType& view)
        : Handle{CopyFunc(view.get())}
    {
    }

    ~Handle() { DestroyFunc(m_ptr); }

    CType* get() { return m_ptr; }
    const CType* get() const { return m_ptr; }
};

template <typename CType, void (*DestroyFunc)(CType*)>
class UniqueHandle
{
protected:
    struct Deleter {
        void operator()(CType* ptr) const noexcept
        {
            if (ptr) DestroyFunc(ptr);
        }
    };
    std::unique_ptr<CType, Deleter> m_ptr;

public:
    explicit UniqueHandle(CType* ptr) : m_ptr{check(ptr)} {}

    CType* get() { return m_ptr.get(); }
    const CType* get() const { return m_ptr.get(); }
};

class PrecomputedTransactionData;
class Transaction;
class TransactionOutput;

template <typename Derived>
class ScriptPubkeyApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    ScriptPubkeyApi() = default;

public:
    bool Verify(int64_t amount,
                const Transaction& tx_to,
                const PrecomputedTransactionData* precomputed_txdata,
                unsigned int input_index,
                ScriptVerificationFlags flags,
                ScriptVerifyStatus& status) const;

    std::vector<std::byte> ToBytes() const
    {
        return write_bytes(impl(), bc2k_script_pubkey_to_bytes);
    }
};

class ScriptPubkeyView : public View<bc2k_ScriptPubkey>, public ScriptPubkeyApi<ScriptPubkeyView>
{
public:
    explicit ScriptPubkeyView(const bc2k_ScriptPubkey* ptr) : View{ptr} {}
};

class ScriptPubkey : public Handle<bc2k_ScriptPubkey, bc2k_script_pubkey_copy, bc2k_script_pubkey_destroy>, public ScriptPubkeyApi<ScriptPubkey>
{
public:
    explicit ScriptPubkey(std::span<const std::byte> raw)
        : Handle{bc2k_script_pubkey_create(raw.data(), raw.size())} {}

    ScriptPubkey(const ScriptPubkeyView& view)
        : Handle(view) {}
};

template <typename Derived>
class TransactionOutputApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    TransactionOutputApi() = default;

public:
    int64_t Amount() const
    {
        return bc2k_transaction_output_get_amount(impl());
    }

    ScriptPubkeyView GetScriptPubkey() const
    {
        return ScriptPubkeyView{bc2k_transaction_output_get_script_pubkey(impl())};
    }
};

class TransactionOutputView : public View<bc2k_TransactionOutput>, public TransactionOutputApi<TransactionOutputView>
{
public:
    explicit TransactionOutputView(const bc2k_TransactionOutput* ptr) : View{ptr} {}
};

class TransactionOutput : public Handle<bc2k_TransactionOutput, bc2k_transaction_output_copy, bc2k_transaction_output_destroy>, public TransactionOutputApi<TransactionOutput>
{
public:
    explicit TransactionOutput(const ScriptPubkey& script_pubkey, int64_t amount)
        : Handle{bc2k_transaction_output_create(script_pubkey.get(), amount)} {}

    TransactionOutput(const TransactionOutputView& view)
        : Handle(view) {}
};

template <typename Derived>
class TxidApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    TxidApi() = default;

public:
    bool operator==(const TxidApi& other) const
    {
        return bc2k_txid_equals(impl(), other.impl()) != 0;
    }

    bool operator!=(const TxidApi& other) const
    {
        return bc2k_txid_equals(impl(), other.impl()) == 0;
    }

    std::array<std::byte, 32> ToBytes() const
    {
        std::array<std::byte, 32> hash;
        bc2k_txid_to_bytes(impl(), reinterpret_cast<unsigned char*>(hash.data()));
        return hash;
    }
};

class TxidView : public View<bc2k_Txid>, public TxidApi<TxidView>
{
public:
    explicit TxidView(const bc2k_Txid* ptr) : View{ptr} {}
};

class Txid : public Handle<bc2k_Txid, bc2k_txid_copy, bc2k_txid_destroy>, public TxidApi<Txid>
{
public:
    Txid(const TxidView& view)
        : Handle(view) {}
};

template <typename Derived>
class OutPointApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    OutPointApi() = default;

public:
    uint32_t index() const
    {
        return bc2k_transaction_out_point_get_index(impl());
    }

    TxidView Txid() const
    {
        return TxidView{bc2k_transaction_out_point_get_txid(impl())};
    }
};

class OutPointView : public View<bc2k_TransactionOutPoint>, public OutPointApi<OutPointView>
{
public:
    explicit OutPointView(const bc2k_TransactionOutPoint* ptr) : View{ptr} {}
};

class OutPoint : public Handle<bc2k_TransactionOutPoint, bc2k_transaction_out_point_copy, bc2k_transaction_out_point_destroy>, public OutPointApi<OutPoint>
{
public:
    OutPoint(const OutPointView& view)
        : Handle(view) {}
};

template <typename Derived>
class TransactionInputApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    TransactionInputApi() = default;

public:
    OutPointView OutPoint() const
    {
        return OutPointView{bc2k_transaction_input_get_out_point(impl())};
    }
};

class TransactionInputView : public View<bc2k_TransactionInput>, public TransactionInputApi<TransactionInputView>
{
public:
    explicit TransactionInputView(const bc2k_TransactionInput* ptr) : View{ptr} {}
};

class TransactionInput : public Handle<bc2k_TransactionInput, bc2k_transaction_input_copy, bc2k_transaction_input_destroy>, public TransactionInputApi<TransactionInput>
{
public:
    TransactionInput(const TransactionInputView& view)
        : Handle(view) {}
};

template <typename Derived>
class TransactionApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

public:
    size_t CountOutputs() const
    {
        return bc2k_transaction_count_outputs(impl());
    }

    size_t CountInputs() const
    {
        return bc2k_transaction_count_inputs(impl());
    }

    TransactionOutputView GetOutput(size_t index) const
    {
        return TransactionOutputView{bc2k_transaction_get_output_at(impl(), index)};
    }

    TransactionInputView GetInput(size_t index) const
    {
        return TransactionInputView{bc2k_transaction_get_input_at(impl(), index)};
    }

    TxidView Txid() const
    {
        return TxidView{bc2k_transaction_get_txid(impl())};
    }

    MAKE_RANGE_METHOD(Outputs, Derived, &TransactionApi<Derived>::CountOutputs, &TransactionApi<Derived>::GetOutput, *static_cast<const Derived*>(this))

    MAKE_RANGE_METHOD(Inputs, Derived, &TransactionApi<Derived>::CountInputs, &TransactionApi<Derived>::GetInput, *static_cast<const Derived*>(this))

    std::vector<std::byte> ToBytes() const
    {
        return write_bytes(impl(), bc2k_transaction_to_bytes);
    }
};

class TransactionView : public View<bc2k_Transaction>, public TransactionApi<TransactionView>
{
public:
    explicit TransactionView(const bc2k_Transaction* ptr) : View{ptr} {}
};

class Transaction : public Handle<bc2k_Transaction, bc2k_transaction_copy, bc2k_transaction_destroy>, public TransactionApi<Transaction>
{
public:
    explicit Transaction(std::span<const std::byte> raw_transaction)
        : Handle{bc2k_transaction_create(raw_transaction.data(), raw_transaction.size())} {}

    Transaction(const TransactionView& view)
        : Handle{view} {}
};

class PrecomputedTransactionData : public Handle<bc2k_PrecomputedTransactionData, bc2k_precomputed_transaction_data_copy, bc2k_precomputed_transaction_data_destroy>
{
public:
    explicit PrecomputedTransactionData(const Transaction& tx_to, std::span<const TransactionOutput> spent_outputs)
        : Handle{bc2k_precomputed_transaction_data_create(
            tx_to.get(),
            reinterpret_cast<const bc2k_TransactionOutput**>(
                const_cast<TransactionOutput*>(spent_outputs.data())),
            spent_outputs.size())} {}
};

template <typename Derived>
bool ScriptPubkeyApi<Derived>::Verify(int64_t amount,
                                      const Transaction& tx_to,
                                      const PrecomputedTransactionData* precomputed_txdata,
                                      unsigned int input_index,
                                      ScriptVerificationFlags flags,
                                      ScriptVerifyStatus& status) const
{
    auto result = bc2k_script_pubkey_verify(
        impl(),
        amount,
        tx_to.get(),
        precomputed_txdata ? precomputed_txdata->get() : nullptr,
        input_index,
        static_cast<bc2k_ScriptVerificationFlags>(flags),
        reinterpret_cast<bc2k_ScriptVerifyStatus*>(&status));
    return result == 1;
}

template <typename Derived>
class BlockHashApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

public:
    bool operator==(const Derived& other) const
    {
        return bc2k_block_hash_equals(impl(), other.get()) != 0;
    }

    bool operator!=(const Derived& other) const
    {
        return bc2k_block_hash_equals(impl(), other.get()) == 0;
    }

    std::array<std::byte, 32> ToBytes() const
    {
        std::array<std::byte, 32> hash;
        bc2k_block_hash_to_bytes(impl(), reinterpret_cast<unsigned char*>(hash.data()));
        return hash;
    }
};

class BlockHashView : public View<bc2k_BlockHash>, public BlockHashApi<BlockHashView>
{
public:
    explicit BlockHashView(const bc2k_BlockHash* ptr) : View{ptr} {}
};

class BlockHash : public Handle<bc2k_BlockHash, bc2k_block_hash_copy, bc2k_block_hash_destroy>, public BlockHashApi<BlockHash>
{
public:
    explicit BlockHash(const std::array<std::byte, 32>& hash)
        : Handle{bc2k_block_hash_create(reinterpret_cast<const unsigned char*>(hash.data()))} {}

    explicit BlockHash(bc2k_BlockHash* hash)
        : Handle{hash} {}

    BlockHash(const BlockHashView& view)
        : Handle{view} {}
};

template <typename Derived>
class BlockHeaderApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    BlockHeaderApi() = default;

public:
    BlockHash Hash() const
    {
        return BlockHash{bc2k_block_header_get_hash(impl())};
    }

    BlockHashView PrevHash() const
    {
        return BlockHashView{bc2k_block_header_get_prev_hash(impl())};
    }

    uint32_t Timestamp() const
    {
        return bc2k_block_header_get_timestamp(impl());
    }

    uint32_t Bits() const
    {
        return bc2k_block_header_get_bits(impl());
    }

    int32_t Version() const
    {
        return bc2k_block_header_get_version(impl());
    }

    uint32_t Nonce() const
    {
        return bc2k_block_header_get_nonce(impl());
    }
};

class BlockHeaderView : public View<bc2k_BlockHeader>, public BlockHeaderApi<BlockHeaderView>
{
public:
    explicit BlockHeaderView(const bc2k_BlockHeader* ptr) : View{ptr} {}
};

class BlockHeader : public Handle<bc2k_BlockHeader, bc2k_block_header_copy, bc2k_block_header_destroy>, public BlockHeaderApi<BlockHeader>
{
public:
    explicit BlockHeader(std::span<const std::byte> raw_header)
        : Handle{bc2k_block_header_create(reinterpret_cast<const unsigned char*>(raw_header.data()), raw_header.size())} {}

    BlockHeader(const BlockHeaderView& view)
        : Handle{view} {}

    BlockHeader(bc2k_BlockHeader* header)
        : Handle{header} {}
};

class Block : public Handle<bc2k_Block, bc2k_block_copy, bc2k_block_destroy>
{
public:
    Block(const std::span<const std::byte> raw_block)
        : Handle{bc2k_block_create(raw_block.data(), raw_block.size())}
    {
    }

    Block(bc2k_Block* block) : Handle{block} {}

    size_t CountTransactions() const
    {
        return bc2k_block_count_transactions(get());
    }

    TransactionView GetTransaction(size_t index) const
    {
        return TransactionView{bc2k_block_get_transaction_at(get(), index)};
    }

    MAKE_RANGE_METHOD(Transactions, Block, &Block::CountTransactions, &Block::GetTransaction, *this)

    BlockHash GetHash() const
    {
        return BlockHash{bc2k_block_get_hash(get())};
    }

    BlockHeader GetHeader() const
    {
        return BlockHeader{bc2k_block_get_header(get())};
    }

    std::vector<std::byte> ToBytes() const
    {
        return write_bytes(get(), bc2k_block_to_bytes);
    }
};

inline void logging_disable()
{
    bc2k_logging_disable();
}

inline void logging_set_options(const bc2k_LoggingOptions& logging_options)
{
    bc2k_logging_set_options(logging_options);
}

inline void logging_set_level_category(LogCategory category, LogLevel level)
{
    bc2k_logging_set_level_category(static_cast<bc2k_LogCategory>(category), static_cast<bc2k_LogLevel>(level));
}

inline void logging_enable_category(LogCategory category)
{
    bc2k_logging_enable_category(static_cast<bc2k_LogCategory>(category));
}

inline void logging_disable_category(LogCategory category)
{
    bc2k_logging_disable_category(static_cast<bc2k_LogCategory>(category));
}

template <typename T>
concept Log = requires(T a, std::string_view message) {
    { a.LogMessage(message) } -> std::same_as<void>;
};

template <Log T>
class Logger : UniqueHandle<bc2k_LoggingConnection, bc2k_logging_connection_destroy>
{
public:
    Logger(std::unique_ptr<T> log)
        : UniqueHandle{bc2k_logging_connection_create(
              +[](void* user_data, const char* message, size_t message_len) { static_cast<T*>(user_data)->LogMessage({message, message_len}); },
              log.release(),
              +[](void* user_data) { delete static_cast<T*>(user_data); })}
    {
    }
};

class BlockTreeEntry : public View<bc2k_BlockTreeEntry>
{
public:
    BlockTreeEntry(const bc2k_BlockTreeEntry* entry)
        : View{entry}
    {
    }

    bool operator==(const BlockTreeEntry& other) const
    {
        return bc2k_block_tree_entry_equals(get(), other.get()) != 0;
    }

    std::optional<BlockTreeEntry> GetPrevious() const
    {
        auto entry{bc2k_block_tree_entry_get_previous(get())};
        if (!entry) return std::nullopt;
        return entry;
    }

    int32_t GetHeight() const
    {
        return bc2k_block_tree_entry_get_height(get());
    }

    BlockHashView GetHash() const
    {
        return BlockHashView{bc2k_block_tree_entry_get_block_hash(get())};
    }

    BlockHeader GetHeader() const
    {
        return BlockHeader{bc2k_block_tree_entry_get_block_header(get())};
    }
};

class KernelNotifications
{
public:
    virtual ~KernelNotifications() = default;

    virtual void BlockTipHandler(SynchronizationState state, BlockTreeEntry entry, double verification_progress) {}

    virtual void HeaderTipHandler(SynchronizationState state, int64_t height, int64_t timestamp, bool presync) {}

    virtual void ProgressHandler(std::string_view title, int progress_percent, bool resume_possible) {}

    virtual void WarningSetHandler(Warning warning, std::string_view message) {}

    virtual void WarningUnsetHandler(Warning warning) {}

    virtual void FlushErrorHandler(std::string_view error) {}

    virtual void FatalErrorHandler(std::string_view error) {}
};

template <typename Derived>
class BlockValidationStateApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    BlockValidationStateApi() = default;

public:
    ValidationMode GetValidationMode() const
    {
        return static_cast<ValidationMode>(bc2k_block_validation_state_get_validation_mode(impl()));
    }

    BlockValidationResult GetBlockValidationResult() const
    {
        return static_cast<BlockValidationResult>(bc2k_block_validation_state_get_block_validation_result(impl()));
    }
};

class BlockValidationStateView : public View<bc2k_BlockValidationState>, public BlockValidationStateApi<BlockValidationStateView>
{
public:
    explicit BlockValidationStateView(const bc2k_BlockValidationState* ptr) : View{ptr} {}
};

class BlockValidationState : public Handle<bc2k_BlockValidationState, bc2k_block_validation_state_copy, bc2k_block_validation_state_destroy>, public BlockValidationStateApi<BlockValidationState>
{
public:
    explicit BlockValidationState() : Handle{bc2k_block_validation_state_create()} {}

    BlockValidationState(const BlockValidationStateView& view) : Handle{view} {}
};

class ValidationInterface
{
public:
    virtual ~ValidationInterface() = default;

    virtual void BlockChecked(Block block, BlockValidationStateView state) {}

    virtual void PowValidBlock(BlockTreeEntry entry, Block block) {}

    virtual void BlockConnected(Block block, BlockTreeEntry entry) {}

    virtual void BlockDisconnected(Block block, BlockTreeEntry entry) {}
};

class ChainParams : public Handle<bc2k_ChainParameters, bc2k_chain_parameters_copy, bc2k_chain_parameters_destroy>
{
public:
    ChainParams(ChainType chain_type)
        : Handle{bc2k_chain_parameters_create(static_cast<bc2k_ChainType>(chain_type))} {}
};

class ContextOptions : public UniqueHandle<bc2k_ContextOptions, bc2k_context_options_destroy>
{
public:
    ContextOptions() : UniqueHandle{bc2k_context_options_create()} {}

    void SetChainParams(ChainParams& chain_params)
    {
        bc2k_context_options_set_chainparams(get(), chain_params.get());
    }

    template <typename T>
    void SetNotifications(std::shared_ptr<T> notifications)
    {
        static_assert(std::is_base_of_v<KernelNotifications, T>);
        auto heap_notifications = std::make_unique<std::shared_ptr<T>>(std::move(notifications));
        using user_type = std::shared_ptr<T>*;
        bc2k_context_options_set_notifications(
            get(),
            bc2k_NotificationInterfaceCallbacks{
                .user_data = heap_notifications.release(),
                .user_data_destroy = +[](void* user_data) { delete static_cast<user_type>(user_data); },
                .block_tip = +[](void* user_data, bc2k_SynchronizationState state, const bc2k_BlockTreeEntry* entry, double verification_progress) { (*static_cast<user_type>(user_data))->BlockTipHandler(static_cast<SynchronizationState>(state), BlockTreeEntry{entry}, verification_progress); },
                .header_tip = +[](void* user_data, bc2k_SynchronizationState state, int64_t height, int64_t timestamp, int presync) { (*static_cast<user_type>(user_data))->HeaderTipHandler(static_cast<SynchronizationState>(state), height, timestamp, presync == 1); },
                .progress = +[](void* user_data, const char* title, size_t title_len, int progress_percent, int resume_possible) { (*static_cast<user_type>(user_data))->ProgressHandler({title, title_len}, progress_percent, resume_possible == 1); },
                .warning_set = +[](void* user_data, bc2k_Warning warning, const char* message, size_t message_len) { (*static_cast<user_type>(user_data))->WarningSetHandler(static_cast<Warning>(warning), {message, message_len}); },
                .warning_unset = +[](void* user_data, bc2k_Warning warning) { (*static_cast<user_type>(user_data))->WarningUnsetHandler(static_cast<Warning>(warning)); },
                .flush_error = +[](void* user_data, const char* error, size_t error_len) { (*static_cast<user_type>(user_data))->FlushErrorHandler({error, error_len}); },
                .fatal_error = +[](void* user_data, const char* error, size_t error_len) { (*static_cast<user_type>(user_data))->FatalErrorHandler({error, error_len}); },
            });
    }

    template <typename T>
    void SetValidationInterface(std::shared_ptr<T> validation_interface)
    {
        static_assert(std::is_base_of_v<ValidationInterface, T>);
        auto heap_vi = std::make_unique<std::shared_ptr<T>>(std::move(validation_interface));
        using user_type = std::shared_ptr<T>*;
        bc2k_context_options_set_validation_interface(
            get(),
            bc2k_ValidationInterfaceCallbacks{
                .user_data = heap_vi.release(),
                .user_data_destroy = +[](void* user_data) { delete static_cast<user_type>(user_data); },
                .block_checked = +[](void* user_data, bc2k_Block* block, const bc2k_BlockValidationState* state) { (*static_cast<user_type>(user_data))->BlockChecked(Block{block}, BlockValidationStateView{state}); },
                .pow_valid_block = +[](void* user_data, bc2k_Block* block, const bc2k_BlockTreeEntry* entry) { (*static_cast<user_type>(user_data))->PowValidBlock(BlockTreeEntry{entry}, Block{block}); },
                .block_connected = +[](void* user_data, bc2k_Block* block, const bc2k_BlockTreeEntry* entry) { (*static_cast<user_type>(user_data))->BlockConnected(Block{block}, BlockTreeEntry{entry}); },
                .block_disconnected = +[](void* user_data, bc2k_Block* block, const bc2k_BlockTreeEntry* entry) { (*static_cast<user_type>(user_data))->BlockDisconnected(Block{block}, BlockTreeEntry{entry}); },
            });
    }
};

class Context : public Handle<bc2k_Context, bc2k_context_copy, bc2k_context_destroy>
{
public:
    Context(ContextOptions& opts)
        : Handle{bc2k_context_create(opts.get())} {}

    Context()
        : Handle{bc2k_context_create(ContextOptions{}.get())} {}

    bool interrupt()
    {
        return bc2k_context_interrupt(get()) == 0;
    }
};

class ChainstateManagerOptions : public UniqueHandle<bc2k_ChainstateManagerOptions, bc2k_chainstate_manager_options_destroy>
{
public:
    ChainstateManagerOptions(const Context& context, std::string_view data_dir, std::string_view blocks_dir)
        : UniqueHandle{bc2k_chainstate_manager_options_create(
              context.get(), data_dir.data(), data_dir.length(), blocks_dir.data(), blocks_dir.length())}
    {
    }

    void SetWorkerThreads(int worker_threads)
    {
        bc2k_chainstate_manager_options_set_worker_threads_num(get(), worker_threads);
    }

    bool SetWipeDbs(bool wipe_block_tree, bool wipe_chainstate)
    {
        return bc2k_chainstate_manager_options_set_wipe_dbs(get(), wipe_block_tree, wipe_chainstate) == 0;
    }

    void UpdateBlockTreeDbInMemory(bool block_tree_db_in_memory)
    {
        bc2k_chainstate_manager_options_update_block_tree_db_in_memory(get(), block_tree_db_in_memory);
    }

    void UpdateChainstateDbInMemory(bool chainstate_db_in_memory)
    {
        bc2k_chainstate_manager_options_update_chainstate_db_in_memory(get(), chainstate_db_in_memory);
    }
};

class ChainView : public View<bc2k_Chain>
{
public:
    explicit ChainView(const bc2k_Chain* ptr) : View{ptr} {}

    int32_t Height() const
    {
        return bc2k_chain_get_height(get());
    }

    int CountEntries() const
    {
        return bc2k_chain_get_height(get()) + 1;
    }

    BlockTreeEntry GetByHeight(int height) const
    {
        auto index{bc2k_chain_get_by_height(get(), height)};
        if (!index) throw std::runtime_error("No entry in the chain at the provided height");
        return index;
    }

    bool Contains(BlockTreeEntry& entry) const
    {
        return bc2k_chain_contains(get(), entry.get());
    }

    MAKE_RANGE_METHOD(Entries, ChainView, &ChainView::CountEntries, &ChainView::GetByHeight, *this)
};

template <typename Derived>
class CoinApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    CoinApi() = default;

public:
    uint32_t GetConfirmationHeight() const { return bc2k_coin_confirmation_height(impl()); }

    bool IsCoinbase() const { return bc2k_coin_is_coinbase(impl()) == 1; }

    TransactionOutputView GetOutput() const
    {
        return TransactionOutputView{bc2k_coin_get_output(impl())};
    }
};

class CoinView : public View<bc2k_Coin>, public CoinApi<CoinView>
{
public:
    explicit CoinView(const bc2k_Coin* ptr) : View{ptr} {}
};

class Coin : public Handle<bc2k_Coin, bc2k_coin_copy, bc2k_coin_destroy>, public CoinApi<Coin>
{
public:
    Coin(bc2k_Coin* coin) : Handle{coin} {}

    Coin(const CoinView& view) : Handle{view} {}
};

template <typename Derived>
class TransactionSpentOutputsApi
{
private:
    auto impl() const
    {
        return static_cast<const Derived*>(this)->get();
    }

    friend Derived;
    TransactionSpentOutputsApi() = default;

public:
    size_t Count() const
    {
        return bc2k_transaction_spent_outputs_count(impl());
    }

    CoinView GetCoin(size_t index) const
    {
        return CoinView{bc2k_transaction_spent_outputs_get_coin_at(impl(), index)};
    }

    MAKE_RANGE_METHOD(Coins, Derived, &TransactionSpentOutputsApi<Derived>::Count, &TransactionSpentOutputsApi<Derived>::GetCoin, *static_cast<const Derived*>(this))
};

class TransactionSpentOutputsView : public View<bc2k_TransactionSpentOutputs>, public TransactionSpentOutputsApi<TransactionSpentOutputsView>
{
public:
    explicit TransactionSpentOutputsView(const bc2k_TransactionSpentOutputs* ptr) : View{ptr} {}
};

class TransactionSpentOutputs : public Handle<bc2k_TransactionSpentOutputs, bc2k_transaction_spent_outputs_copy, bc2k_transaction_spent_outputs_destroy>,
                                public TransactionSpentOutputsApi<TransactionSpentOutputs>
{
public:
    TransactionSpentOutputs(bc2k_TransactionSpentOutputs* transaction_spent_outputs) : Handle{transaction_spent_outputs} {}

    TransactionSpentOutputs(const TransactionSpentOutputsView& view) : Handle{view} {}
};

class BlockSpentOutputs : public Handle<bc2k_BlockSpentOutputs, bc2k_block_spent_outputs_copy, bc2k_block_spent_outputs_destroy>
{
public:
    BlockSpentOutputs(bc2k_BlockSpentOutputs* block_spent_outputs)
        : Handle{block_spent_outputs}
    {
    }

    size_t Count() const
    {
        return bc2k_block_spent_outputs_count(get());
    }

    TransactionSpentOutputsView GetTxSpentOutputs(size_t tx_undo_index) const
    {
        return TransactionSpentOutputsView{bc2k_block_spent_outputs_get_transaction_spent_outputs_at(get(), tx_undo_index)};
    }

    MAKE_RANGE_METHOD(TxsSpentOutputs, BlockSpentOutputs, &BlockSpentOutputs::Count, &BlockSpentOutputs::GetTxSpentOutputs, *this)
};

class ChainMan : UniqueHandle<bc2k_ChainstateManager, bc2k_chainstate_manager_destroy>
{
public:
    ChainMan(const Context& context, const ChainstateManagerOptions& chainman_opts)
        : UniqueHandle{bc2k_chainstate_manager_create(chainman_opts.get())}
    {
    }

    bool ImportBlocks(const std::span<const std::string> paths)
    {
        std::vector<const char*> c_paths;
        std::vector<size_t> c_paths_lens;
        c_paths.reserve(paths.size());
        c_paths_lens.reserve(paths.size());
        for (const auto& path : paths) {
            c_paths.push_back(path.c_str());
            c_paths_lens.push_back(path.length());
        }

        return bc2k_chainstate_manager_import_blocks(get(), c_paths.data(), c_paths_lens.data(), c_paths.size()) == 0;
    }

    bool ProcessBlock(const Block& block, bool* new_block)
    {
        int _new_block;
        int res = bc2k_chainstate_manager_process_block(get(), block.get(), &_new_block);
        if (new_block) *new_block = _new_block == 1;
        return res == 0;
    }

    bool ProcessBlockHeader(const BlockHeader& header, BlockValidationState& state)
    {
        return bc2k_chainstate_manager_process_block_header(get(), header.get(), state.get()) == 0;
    }

    ChainView GetChain() const
    {
        return ChainView{bc2k_chainstate_manager_get_active_chain(get())};
    }

    std::optional<BlockTreeEntry> GetBlockTreeEntry(const BlockHash& block_hash) const
    {
        auto entry{bc2k_chainstate_manager_get_block_tree_entry_by_hash(get(), block_hash.get())};
        if (!entry) return std::nullopt;
        return entry;
    }

    BlockTreeEntry GetBestEntry() const
    {
        return bc2k_chainstate_manager_get_best_entry(get());
    }

    std::optional<Block> ReadBlock(const BlockTreeEntry& entry) const
    {
        auto block{bc2k_block_read(get(), entry.get())};
        if (!block) return std::nullopt;
        return block;
    }

    BlockSpentOutputs ReadBlockSpentOutputs(const BlockTreeEntry& entry) const
    {
        return bc2k_block_spent_outputs_read(get(), entry.get());
    }
};

} // namespace bc2k

#endif // BITCOINII_KERNEL_BITCOINIIKERNEL_WRAPPER_H
