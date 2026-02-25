# Sapphire Use Cases
## One Language for Every Domain

### 🌐 Web Development (Full-Stack)

#### Frontend (WASM)
```sapphire
# Compiles to WebAssembly
import web::{dom, events, fetch}

@component
class TodoApp:
    state: List<Todo>
    
    fn render(self) -> Element:
        return div(class="container"):
            h1("My Todos")
            for todo in self.state:
                TodoItem(todo)
            
    @event("click")
    fn add_todo(self, text: str):
        self.state.append(Todo(text))
        self.render()
```

#### Backend (Web Server)
```sapphire
import http::{Server, Request, Response, Router}
import db::{postgres, query}

let app = Router()

@app.get("/users/:id")
async fn get_user(req: Request) -> Response:
    let user_id = req.params["id"].parse<int>()?
    let user = await db.query("SELECT * FROM users WHERE id = $1", user_id)
    return Response.json(user)

@app.post("/users")
async fn create_user(req: Request) -> Response:
    let data = req.json<UserCreate>()?
    let user = await User.create(data)
    return Response.json(user, status=201)

async fn main():
    let server = Server.new("0.0.0.0:8000")
    server.use(app)
    await server.listen()
```

### ⚙️ Systems Programming

#### Operating System Components
```sapphire
import sys::{memory, process, syscall}

# Low-level memory management
unsafe fn allocate_pages(count: usize) -> *mut u8:
    let ptr = syscall.mmap(
        null,
        count * PAGE_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS
    )
    return ptr as *mut u8

# Device driver
class NetworkDriver:
    impl Driver:
        fn init(self) -> Result<(), Error>:
            # Direct hardware access
            unsafe:
                let base_addr = 0xFEBC0000 as *mut u32
                *base_addr = INIT_COMMAND
            return Ok(())
        
        fn read(self, buffer: &mut [u8]) -> Result<usize, Error>:
            # DMA transfer
            unsafe:
                dma_transfer(self.device_addr, buffer.as_ptr(), buffer.len())
```

#### Kernel Module
```sapphire
@kernel_module
module filesystem:
    fn mount(device: str, mountpoint: str) -> Result<(), Error>:
        let superblock = read_superblock(device)?
        let fs = Filesystem.new(superblock)
        register_filesystem(mountpoint, fs)
        return Ok(())
```

### 🎮 Game Development

```sapphire
import game::{Engine, Sprite, Physics, Input}

class Player:
    position: Vec2
    velocity: Vec2
    sprite: Sprite
    
    fn update(self, delta: float):
        # Input handling
        if Input.is_pressed(Key.Left):
            self.velocity.x = -200.0
        elif Input.is_pressed(Key.Right):
            self.velocity.x = 200.0
        
        # Physics
        self.position += self.velocity * delta
        
        # Collision detection
        for enemy in enemies:
            if self.sprite.collides_with(enemy.sprite):
                self.take_damage(10)
    
    fn render(self):
        self.sprite.draw(self.position)

async fn main():
    let engine = Engine.new(width=1920, height=1080, fps=60)
    let player = Player.new()
    
    engine.run():
        player.update(engine.delta_time)
        player.render()
```

### 📊 Data Science & Machine Learning

```sapphire
import ml::{tensor, nn, optim}
import data::{DataFrame, Series}

# Data processing
fn load_and_process_data(path: str) -> DataFrame:
    let df = DataFrame.read_csv(path)
    
    # Pandas-like operations
    return (df
        .filter(lambda row: row["age"] > 18)
        .groupby("category")
        .agg({"sales": "sum", "count": "mean"})
        .sort_by("sales", descending=true))

# Neural network
class NeuralNet:
    impl nn.Module:
        fn __init__(self):
            self.fc1 = nn.Linear(784, 128)
            self.fc2 = nn.Linear(128, 10)
        
        fn forward(self, x: Tensor) -> Tensor:
            x = self.fc1(x).relu()
            return self.fc2(x).softmax()

# Training
async fn train_model():
    let model = NeuralNet()
    let optimizer = optim.Adam(model.parameters(), lr=0.001)
    
    for epoch in range(100):
        for batch in dataloader:
            let (inputs, labels) = batch
            
            # Forward pass
            let outputs = model(inputs)
            let loss = nn.cross_entropy(outputs, labels)
            
            # Backward pass
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
        
        print(f"Epoch {epoch}, Loss: {loss.item()}")
```

### 🔧 DevOps & Infrastructure

```sapphire
import cloud::{aws, docker, k8s}
import infra::{terraform, ansible}

# Infrastructure as Code
fn provision_infrastructure():
    let vpc = aws.VPC(
        cidr="10.0.0.0/16",
        tags={"Name": "production-vpc"}
    )
    
    let subnet = aws.Subnet(
        vpc_id=vpc.id,
        cidr="10.0.1.0/24",
        availability_zone="us-east-1a"
    )
    
    let instance = aws.EC2Instance(
        ami="ami-12345678",
        instance_type="t3.medium",
        subnet_id=subnet.id
    )
    
    return instance

# Container orchestration
fn deploy_application():
    let deployment = k8s.Deployment(
        name="web-app",
        replicas=3,
        containers=[
            k8s.Container(
                name="app",
                image="myapp:latest",
                ports=[8080],
                env={"DATABASE_URL": secret("db-url")}
            )
        ]
    )
    
    let service = k8s.Service(
        name="web-app-service",
        type="LoadBalancer",
        selector={"app": "web-app"},
        ports=[80]
    )
    
    k8s.apply(deployment)
    k8s.apply(service)
```

### 📱 Mobile Development

```sapphire
import mobile::{ui, navigation, storage}

@screen
class HomeScreen:
    state: AppState
    
    fn build(self) -> Widget:
        return Scaffold(
            appBar=AppBar(title="My App"),
            body=Column(
                children=[
                    Text("Welcome!", style=TextStyle(size=24)),
                    Button(
                        text="Click Me",
                        onPressed=self.handle_click
                    ),
                    ListView(
                        items=self.state.items,
                        builder=lambda item: ListTile(title=item.name)
                    )
                ]
            )
        )
    
    async fn handle_click(self):
        let result = await api.fetch_data()
        self.state.items = result
        self.rebuild()

# Cross-platform: iOS, Android, Desktop
async fn main():
    let app = MobileApp(home=HomeScreen())
    app.run()
```

### 🔐 Blockchain & Cryptography

```sapphire
import crypto::{hash, signature, encryption}
import blockchain::{Block, Transaction, Consensus}

class Blockchain:
    chain: List<Block>
    pending_transactions: List<Transaction>
    
    fn add_transaction(self, tx: Transaction) -> Result<(), Error>:
        # Verify signature
        if not tx.verify_signature():
            return Err("Invalid signature")
        
        self.pending_transactions.append(tx)
        return Ok(())
    
    fn mine_block(self, miner_address: str) -> Block:
        let block = Block(
            index=len(self.chain),
            transactions=self.pending_transactions,
            previous_hash=self.chain[-1].hash,
            timestamp=time.now()
        )
        
        # Proof of work
        while not block.hash.startswith("0000"):
            block.nonce += 1
            block.hash = block.calculate_hash()
        
        self.chain.append(block)
        self.pending_transactions = []
        return block

# Smart contract
@contract
class Token:
    balances: Map<Address, uint256>
    
    @payable
    fn transfer(self, to: Address, amount: uint256):
        require(self.balances[msg.sender] >= amount, "Insufficient balance")
        self.balances[msg.sender] -= amount
        self.balances[to] += amount
        emit Transfer(msg.sender, to, amount)
```

### 🧪 Scientific Computing

```sapphire
import sci::{numpy, scipy, matplotlib}
import parallel::{mpi, cuda}

# GPU-accelerated computation
@cuda.kernel
fn matrix_multiply(A: Tensor, B: Tensor, C: Tensor):
    let row = cuda.threadIdx.x + cuda.blockIdx.x * cuda.blockDim.x
    let col = cuda.threadIdx.y + cuda.blockIdx.y * cuda.blockDim.y
    
    if row < A.shape[0] and col < B.shape[1]:
        let sum = 0.0
        for k in range(A.shape[1]):
            sum += A[row, k] * B[k, col]
        C[row, col] = sum

# Parallel processing
fn simulate_physics(particles: List<Particle>) -> List<Particle>:
    # Distribute across MPI nodes
    let comm = mpi.COMM_WORLD
    let rank = comm.rank
    let size = comm.size
    
    let chunk = particles[rank::size]
    let results = [update_particle(p) for p in chunk]
    
    # Gather results
    return comm.gather(results, root=0)
```

### 🤖 IoT & Embedded Systems

```sapphire
import embedded::{gpio, i2c, spi, uart}
import rtos::{Task, Mutex, Queue}

# Bare metal embedded
@no_std
@target("arm-cortex-m4")
class TemperatureSensor:
    i2c: I2C
    address: u8
    
    fn read_temperature(self) -> Result<float, Error>:
        let data = self.i2c.read(self.address, 2)?
        let temp = (data[0] as u16) << 8 | data[1] as u16
        return Ok(temp as float / 100.0)

# RTOS task
@task(priority=5, stack_size=2048)
async fn sensor_task():
    let sensor = TemperatureSensor.new()
    let queue = Queue<float>.new(10)
    
    loop:
        let temp = sensor.read_temperature().await?
        queue.send(temp).await
        delay_ms(1000)
```

### 🔬 Bioinformatics

```sapphire
import bio::{sequence, alignment, phylogeny}

fn analyze_genome(fasta_file: str) -> Analysis:
    let sequences = bio.read_fasta(fasta_file)
    
    # Sequence alignment
    let aligned = bio.align_multiple(sequences, algorithm="clustal")
    
    # Find conserved regions
    let conserved = aligned.find_conserved_regions(threshold=0.9)
    
    # Phylogenetic tree
    let tree = bio.build_tree(aligned, method="neighbor_joining")
    
    return Analysis(aligned, conserved, tree)
```

### 🎵 Audio/Video Processing

```sapphire
import media::{audio, video, codec}

async fn process_video(input: str, output: str):
    let video = video.open(input)
    
    # Process frames in parallel
    let processed = await video.frames()
        .parallel_map(lambda frame: 
            frame
                .resize(1920, 1080)
                .apply_filter("sharpen")
                .adjust_brightness(1.2)
        )
    
    # Encode with H.265
    let encoder = codec.H265Encoder(bitrate="5M", preset="slow")
    encoder.encode(processed, output)
```

### 🌍 GIS & Mapping

```sapphire
import geo::{Point, Polygon, Map}
import spatial::{distance, intersection}

fn find_nearby_locations(center: Point, radius: float) -> List<Location>:
    let locations = db.query<Location>(
        "SELECT * FROM locations WHERE ST_DWithin(geom, $1, $2)",
        center, radius
    )
    
    return locations
        .filter(lambda loc: loc.is_open())
        .sort_by(lambda loc: distance(center, loc.position))
```
