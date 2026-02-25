# Sapphire Standard Library

## Core Modules

### `std.collections`
- `List<T>` - Dynamic array
- `Dict<K, V>` - Hash map
- `Set<T>` - Hash set
- `Queue<T>` - FIFO queue
- `Stack<T>` - LIFO stack
- `Deque<T>` - Double-ended queue
- `Heap<T>` - Priority queue
- `Tree<T>` - Binary search tree

### `std.io`
- `File` - File operations
- `Directory` - Directory operations
- `Path` - Path manipulation
- `Stream` - Byte streams
- `Reader` / `Writer` - I/O traits
- `BufferedReader` / `BufferedWriter`

### `std.async`
- `Future<T>` - Asynchronous computation
- `Promise<T>` - Promise pattern
- `Task` - Async task
- `Executor` - Task executor
- `Runtime` - Async runtime

### `std.sync`
- `Mutex<T>` - Mutual exclusion
- `RwLock<T>` - Read-write lock
- `Semaphore` - Counting semaphore
- `Barrier` - Synchronization barrier
- `Channel<T>` - Message passing
- `Atomic<T>` - Atomic operations

### `std.string`
- `String` - UTF-8 string
- `StringBuilder` - Efficient string building
- `Regex` - Regular expressions
- `Format` - String formatting

### `std.math`
- Basic: `abs`, `min`, `max`, `clamp`
- Trigonometry: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`
- Exponential: `exp`, `log`, `log10`, `pow`, `sqrt`
- Rounding: `floor`, `ceil`, `round`, `trunc`
- Constants: `PI`, `E`, `TAU`

### `std.time`
- `DateTime` - Date and time
- `Duration` - Time duration
- `Instant` - Monotonic time
- `Timer` - Timer utilities
- `sleep` / `sleep_until`

### `std.random`
- `Random` - Random number generator
- `thread_rng()` - Thread-local RNG
- `range(min, max)` - Random in range
- `choice(list)` - Random selection
- `shuffle(list)` - Shuffle in place

## Domain-Specific Modules

### `std.http`
- `Client` - HTTP client
- `Server` - HTTP server
- `Request` / `Response`
- `Router` - URL routing
- `WebSocket` - WebSocket support
- `middleware` - Common middleware

### `std.json`
- `parse(str)` - Parse JSON
- `dumps(obj)` - Serialize to JSON
- `loads(str)` - Deserialize from JSON
- `JsonValue` - JSON value type

### `std.xml`
- `parse(str)` - Parse XML
- `Element` - XML element
- `XPath` - XPath queries

### `std.crypto`
- `hash` - Hashing (SHA256, SHA512, etc.)
- `hmac` - HMAC
- `aes` - AES encryption
- `rsa` - RSA encryption
- `jwt` - JSON Web Tokens
- `random_bytes` - Cryptographically secure random

### `std.db`
- `Connection` - Database connection
- `Query` - Query builder
- `Transaction` - Database transaction
- `Model` - ORM base class
- Drivers: `postgres`, `mysql`, `sqlite`, `mongodb`

### `std.test`
- `@test` - Test decorator
- `assert_eq` / `assert_ne`
- `assert_true` / `assert_false`
- `assert_raises`
- `mock` - Mocking utilities
- `TestSuite` - Test suite

### `std.log`
- `Logger` - Logger instance
- `debug`, `info`, `warn`, `error`, `critical`
- `FileHandler` - Log to file
- `ConsoleHandler` - Log to console
- `RotatingFileHandler` - Rotating logs

## Web Development

### `std.web`
- `html` - HTML generation
- `css` - CSS utilities
- `dom` - DOM manipulation (WASM)
- `fetch` - Fetch API
- `storage` - Local/session storage

### `std.template`
- `Template` - Template engine
- `render(template, context)`
- Jinja2-like syntax

## Data Science

### `std.data`
- `DataFrame` - Pandas-like dataframe
- `Series` - Data series
- `read_csv` / `to_csv`
- `read_json` / `to_json`
- `groupby`, `pivot`, `merge`

### `std.ml`
- `tensor` - Tensor operations
- `nn` - Neural network layers
- `optim` - Optimizers
- `metrics` - Evaluation metrics
- `datasets` - Common datasets

### `std.plot`
- `figure` / `subplot`
- `plot`, `scatter`, `bar`, `hist`
- `heatmap`, `boxplot`
- `xlabel`, `ylabel`, `title`, `legend`
- `show`, `save`

## Systems Programming

### `std.sys`
- `memory` - Memory operations
- `process` - Process management
- `syscall` - System calls
- `signal` - Signal handling
- `env` - Environment variables

### `std.os`
- `platform` - Platform detection
- `arch` - Architecture info
- `cpu_count` - CPU count
- `memory_info` - Memory info

## Networking

### `std.net`
- `TcpListener` / `TcpStream`
- `UdpSocket`
- `IpAddr` - IP address
- `SocketAddr` - Socket address
- `dns` - DNS resolution

## Compression

### `std.compress`
- `gzip` - Gzip compression
- `zlib` - Zlib compression
- `bzip2` - Bzip2 compression
- `tar` - Tar archives
- `zip` - Zip archives

## Serialization

### `std.serde`
- `Serialize` / `Deserialize` traits
- `to_bytes` / `from_bytes`
- `to_string` / `from_string`

## CLI

### `std.cli`
- `Command` - CLI command
- `Arg` - Command argument
- `Flag` - Command flag
- `parse_args` - Argument parsing

### `std.term`
- `color` - Terminal colors
- `style` - Text styling
- `progress` - Progress bars
- `prompt` - User prompts
- `clear`, `cursor` - Terminal control

## Multimedia

### `std.image`
- `Image` - Image type
- `load` / `save`
- `resize`, `crop`, `rotate`
- `filter` - Image filters

### `std.audio`
- `Audio` - Audio type
- `load` / `save`
- `play`, `record`
- `effects` - Audio effects

### `std.video`
- `Video` - Video type
- `load` / `save`
- `frames` - Frame iterator
- `codec` - Video codecs
