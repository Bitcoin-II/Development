BitcoinII
============

https://github.com/Bitcoin-II/BitcoinII-Core

BitcoinII is a BitcoinII full-node implementation derived from BitcoinII Core,
with a focus on database performance, storage efficiency, node configurability,
and selected forward-looking performance improvements.

BitcoinII v31.1.1 is based on BitcoinII Core v31.1 and remains compatible with
the BitcoinII network and BitcoinII consensus rules.

This initial BitcoinII release is provided as source code. Pre-built release
binaries are not currently provided.

What is BitcoinII?
---------------------

BitcoinII connects to the BitcoinII peer-to-peer network to download and fully
validate blocks and transactions. It also includes the BitcoinII wallet, RPC
interface, command-line utilities, and graphical user interface.

BitcoinII is not a separate cryptocurrency and does not define a separate
blockchain. It is an alternative BitcoinII node implementation built from the
BitcoinII Core codebase.

The primary changes in BitcoinII v31.1.1 include:

- Replacement of the LevelDB database backend with RocksDB.
- Hardware-aware automatic database-cache configuration.
- Workload-specific RocksDB tuning for chainstate and optional indexes.
- LZ4 and Zstandard compression within the RocksDB storage backend.
- Zstandard-compressed `blk*.dat` block records with transparent raw-record
  compatibility and fallback when compression is not beneficial.
- Reduced disk usage for blockchain and index storage.
- Parallel block-input prevout fetching during block validation, backported
  from post-v31.1 BitcoinII Core development.
- Consensus-enforced restrictions on arbitrary blockchain data storage.
- BitcoinII-specific branding, configuration naming, and application
  integration.

Further technical information is available in the [doc folder](doc/).

BitcoinII-Specific Documentation
-----------------------------------

The following documents describe significant BitcoinII-specific behavior:

- [RocksDB automatic configuration](doc/BitcoinII-RocksDB-AutoConfig-Documentation.md)
- [BitcoinII v31.1.1 disk storage comparison](doc/BitcoinII-31.1.1-Disk-Storage-Usage-Comparison-Results.md)
- [Parallel prevout fetching](doc/BitcoinII-Parallel-Prevout-Fetch.md)

RocksDB and Build Dependencies
------------------------------

BitcoinII replaces BitcoinII Core's LevelDB backend with RocksDB and uses LZ4
and Zstandard as part of its database and block-storage implementation.

Users building BitcoinII from source are strongly encouraged to use the
BitcoinII Depends system. The source tree specifies the RocksDB, LZ4, and
Zstandard versions and build configuration against which BitcoinII is
developed and tested.

See the platform-specific build documentation in the [doc folder](doc/) and
the [depends documentation](depends/README.md) for additional information.

Configuration
-------------

The primary BitcoinII configuration file is:

    bitcoinII.conf

BitcoinII retains the familiar BitcoinII Core configuration model and
command-line option format while adding BitcoinII-specific functionality.

Database memory is automatically selected according to available system
resources when `dbcache` is not explicitly configured. Users who prefer a
manual database-memory budget may continue to set `dbcache` explicitly.

See the
[RocksDB automatic configuration documentation](doc/BitcoinII-RocksDB-AutoConfig-Documentation.md)
for details.

Development
-----------

The `main` branch contains the current BitcoinII development and release
history.

Official source release points are identified with version tags such as:

    v31.1.1

BitcoinII is derived from BitcoinII Core and continues to incorporate relevant
upstream BitcoinII Core development while maintaining the BitcoinII-specific
storage, database, performance, policy, and application changes.

The contribution workflow is described in
[CONTRIBUTING.md](CONTRIBUTING.md), and developer information can be found in
[doc/developer-notes.md](doc/developer-notes.md).

Testing
-------

BitcoinII inherits BitcoinII Core's extensive unit, functional, fuzz, and
integration testing infrastructure and adds or modifies tests where required
for BitcoinII-specific functionality.

Unit tests can be compiled and executed with CTest when tests were enabled
during build configuration:

    ctest

Further information about unit tests is available in
[src/test/README.md](src/test/README.md).

Functional and integration tests are located under [test/](test/) and can be
run using the functional test runner from the configured build tree.

BitcoinII modifies security-critical BitcoinII software. Changes should be
reviewed and tested carefully, particularly changes affecting validation,
database handling, transaction policy, block storage, or wallet behavior.

Upstream BitcoinII Core
---------------------

BitcoinII is derived from the BitcoinII Core project:

https://bitcoincore.org

BitcoinII Core source code is available at:

https://github.com/bitcoin/bitcoin

BitcoinII retains substantial BitcoinII Core code, documentation, testing
infrastructure, and copyright attribution.

License
-------

BitcoinII is released under the terms of the MIT license.

See [COPYING](COPYING) for the full license text or:

https://opensource.org/license/MIT

Source Repository
-----------------

The official BitcoinII source repository is:

https://github.com/Bitcoin-II/BitcoinII-Core

Issues and source-development reports may be submitted through:

https://github.com/Bitcoin-II/BitcoinII-Core/issues

Donations
---------

If you have found BitcoinII to be useful to yourself, your organization, or
to the broader BitcoinII ecosystem, please consider assisting in furthering the
development and maintenance of BitcoinII by helping the developer stay in a
steady supply of coffee.

BC2: 1A1gc5mi9N4Dth7QVCiffF2Cuy1yXAadbp
