# 📈 Stock Market Simulator

## 🚀 Overview

I built this stock market simulator (`market`) to explore order matching in a virtual exchange. It uses `std::priority_queue` to process buy/sell orders from standard input, with customizable output via command-line flags. Aimed to boost my C++ STL and OOP skills.

⏳ *Self-imposed deadline: October 11, 2024, 11:59 PM.*

---

## ✨ Features

- ✅ Matches orders by price and timestamp.
- 📥 Input: Trade List (TL) or Pseudorandom (PR) via `P2random.h`.
- 🔧 Flags: `-v` (verbose), `-m` (median), `-i` (trader info), `-t` (time travelers).

### 🖥️ Example Usage
```bash
./market -vmit < input.txt > output.txt
```

---

## 🔍 Market Logic

Orders (timestamp, trader ID, stock ID, buy/sell, price, quantity) match if:
- 📊 Same stock, opposite intents, seller’s price ≤ buyer’s.
- 📌 Price favors the earlier order; ties break by timestamp.

#### 📄 Sample
```
0 BUY T1 S2 $100 #10
1 SELL T2 S2 $10 #20
```
🎯 T2 sells 10 to T1 at $100, posts 10 more at $10.

---

## 📜 Input

**📝 Header:**
```
COMMENT: Wild market test
MODE: TL
NUM_TRADERS: 5
NUM_STOCKS: 3
```

**📥 TL:** `0 BUY T1 S2 $100 #50`  
**🎲 PR:** Seed, order count, arrival rate.

---

## 📤 Output

**Always:**  
```
Processing orders...
---End of Day---
Trades Completed: 5
```

**Optional:**  
- 📢 **Verbose:** “Trader 1 bought 10 shares…”  
- 📊 **Median:** “Median match price of Stock 0…”  
- 🏦 **Trader Info:** “Trader 1 bought 20, sold 10…”  
- ⏳ **Time Travelers:** “Buy Stock 0 at time 1 for $10…”

---

## 🔧 Build

- **🔗 Requires:** `g++`, STL, `-O3` in `Makefile`.
- **📂 Files:** `market.cpp`, `P2random.h/cpp`, `Makefile`, `test-*.txt`.
- **▶️ Run:** `make -R -r`.

🆔 **Project ID:**  
```cpp
// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6
```

---

## 🧪 Testing

🛠️ Test files (`test-<N>-<v/m/t/i>.txt`, max 30 orders) catch bugs like invalid inputs or leaks.

---

## 🎯 Takeaways

- 💡 Mastered `std::priority_queue`.
- 📖 Learned efficient input parsing.
- ⚡ Optimized with `-O3`.

🗓️ *Updated: March 16, 2025*

--- 

This cuts the fluff, keeps the core, and still feels like my own work. Thoughts? 🤔
