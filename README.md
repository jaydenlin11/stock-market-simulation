# 📈 Stock Market Simulator

## 🔧 Overview

I crafted this stock market simulator (`market`) to mimic an electronic exchange, matching buy and sell orders with `std::priority_queue`. It reads orders from standard input in two formats and spits out tailored reports based on command-line flags. It’s been a deep dive into C++ STL, object-oriented design, and real-time data handling.

---

## ✨ Features

- ⏳ Matches orders instantly using priority queues.
- 📅 Input: Trade List (TL) or Pseudorandom (PR) via `P2random.h`.
- 🔢 Flags: `-v` (verbose), `-m` (median), `-i` (trader info), `-t` (time travelers).

### 🛠️ Example Usage
```bash
./market -vmit < input.txt > output.txt
```

---

## 💰 Market Logic

Each order has a timestamp, trader ID, stock ID, buy/sell intent, price limit, and quantity. When a new order arrives, it’s thrown into the market to find matches with existing orders. Here’s how it works:

- **🎯 Matching Criteria:**
  - Same stock ID (e.g., S2).
  - Opposite intents (buy vs. sell; traders can match themselves).
  - Seller’s price ≤ buyer’s price (e.g., sell at $10 matches buy at $20).

- **📊 Price Setting:** The earlier order dictates the trade price. If a sell order at $10 comes first, a later buy at $20 pays $10. If a buy at $20 is first, a sell at $10 fetches $20—maximizing profit for the first mover.

- **🛋️ Priority Queues:** I use `std::priority_queue` to keep sell orders sorted by lowest price and buy orders by highest price. This ensures buyers snag the cheapest available shares, and sellers get the highest bids. Ties (same price) resolve by earliest timestamp.

- **⚖️ Execution:** A new order can match multiple existing ones until its quantity is filled or no matches remain. Completed orders vanish; partial ones linger with updated quantities.

#### 📝 Sample Run
```
0 BUY T1 S2 $100 #10
1 SELL T2 S2 $10 #20
2 BUY T3 S2 $1 #10
```
- 📈 T1 posts a buy for 10 shares at $100 max; no sellers yet.
- 📉 T2 offers 20 shares at $10 min, matches T1’s 10 at $100 (T1’s price), then posts 10 leftover at $10.
- 🚫 T3 bids $1 for 10, but T2’s $10 ask is too high—no deal.

---

## 📄 Input

**📊 Header:**
```
COMMENT: Wild market test
MODE: TL
NUM_TRADERS: 5
NUM_STOCKS: 3
```

**📝 TL Format:** `0 BUY T1 S2 $100 #50`  
**🌟 PR Format:** Seed, order count, arrival rate.

---

## 📈 Output

**Always:**  
```
Processing orders...
---End of Day---
Trades Completed: 5
```

**Optional:**  
- 🔢 Verbose: “Trader 1 bought 10 shares of Stock 2 from Trader 2 for $100/share”  
- 📊 Median: “Median match price of Stock 0 at time 2 is $45”  
- 📅 Trader Info: “Trader 1 bought 20 and sold 10 for a net transfer of $500”  
- ⏲️ Time Travelers: “Buy Stock 0 at time 1 for $10 and sell at time 2 for $40”


*Updated: March 16, 2025*

