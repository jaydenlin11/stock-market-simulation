#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include "P2random.h"
#include <queue>
#include <functional> 
using namespace std;


class Market{
    private:
        struct Order{
            unsigned int ref;
            uint16_t trader_id;
            int price;
            mutable int quantity;
            Order() : ref(0), trader_id(0), price(0), quantity(0) {}
            Order(unsigned int r, uint16_t t, int p, int q) : ref(r), trader_id(t), price(p), quantity(q) {}
        };
        struct Trader{
            int num_sold;
            int num_bought;
            int net_value;
            Trader() : num_sold(0), num_bought(0), net_value(0){}
        };

        struct sortBuy{
            bool operator()(const Order &left, const Order &right) const{
                if(left.price == right.price){
                    return left.ref > right.ref;
                }
                return left.price < right.price;
            }
        };

        struct sortSell{
            bool operator()(const Order &left, const Order &right) const{
                if(left.price == right.price){
                    return left.ref > right.ref;
                }
                return left.price > right.price;
            }
        };

        enum class Timestatus{
            NoTrade,
            CanBuy,
            Compeleted,
            Potential
        };

        class Stock{
            public:
                priority_queue<Order, vector<Order>, sortSell> sellQueue;
                priority_queue<Order, vector<Order>, sortBuy> buyQueue;
                priority_queue<int, vector<int>> firstHalf;
                priority_queue<int, vector<int>, greater<int>> secondHalf;
                Timestatus time_traveler_status = Timestatus::NoTrade;
                int buy_price = 0;
                int sell_price = 0;
                int potential_price = 0;
                unsigned int buy_timestamp;
                unsigned int sell_timestamp;
                unsigned int potential_timestamp;  
        };
        
        vector<Stock> stockList; 
        vector<Trader> traderList;
        unsigned int num_traders;
        unsigned int num_stocks;
        unsigned int trade_completed = 0;
        bool verbose = false;
        bool median = false;
        bool trader_info = false;
        bool time_travelers = false;

        void insertMedian(int trade_price, uint32_t stock_id){
            if (stockList[stock_id].firstHalf.size()<=stockList[stock_id].secondHalf.size()){
                if(stockList[stock_id].secondHalf.empty()||trade_price <= stockList[stock_id].secondHalf.top()){
                    stockList[stock_id].firstHalf.push(trade_price);
                }
                else{
                    stockList[stock_id].secondHalf.push(trade_price);
                    stockList[stock_id].firstHalf.push(stockList[stock_id].secondHalf.top());
                    stockList[stock_id].secondHalf.pop();
                }  
            }
            else{
                if(trade_price >= stockList[stock_id].firstHalf.top()){
                    stockList[stock_id].secondHalf.push(trade_price);
                }
                else{
                    stockList[stock_id].firstHalf.push(trade_price);
                    stockList[stock_id].secondHalf.push(stockList[stock_id].firstHalf.top());
                    stockList[stock_id].firstHalf.pop();
                }  
            }
        }

        void calculateMedian(unsigned int timestamp){
            int median = 0;
            for(size_t i = 0; i < num_stocks;i++){
                size_t firstSize = stockList[i].firstHalf.size();
                size_t secondSize = stockList[i].secondHalf.size();
                if(firstSize!=0||secondSize!=0){
                    if (firstSize==secondSize){
                        median = (stockList[i].firstHalf.top() + stockList[i].secondHalf.top())/2;
                    }
                    else{
                        median = stockList[i].firstHalf.top();
                    }
                    cout << "Median match price of Stock "<< i << " at time "<< timestamp <<" is $"<< median << "\n";
                } 
            }
        }

        void printTraderInfo(){
            cout << "---Trader Info---\n";
            for(size_t i = 0; i < num_traders; i++){
                cout << "Trader " << i << " bought " << traderList[i].num_bought <<" and sold " << traderList[i].num_sold <<" for a net transfer of $" <<traderList[i].net_value << "\n";
            }
        }

        void printTimeTraveler(){
            cout << "---Time Travelers---\n";
            for(size_t i = 0; i < num_stocks; i++){
                Stock &stock = stockList[i];
                if(stock.sell_price==0){
                    cout << "A time traveler could not make a profit on Stock "<<i<<"\n";
                }
                else{
                    cout << "A time traveler would buy Stock "<<i<<" at time "<<stock.buy_timestamp<<" for $"<<stock.buy_price<<" and sell it at time "<<stock.sell_timestamp<<" for $"<<stock.sell_price<<"\n";
                }
            }
        }

        void processOrders(istream &inputStream) {
            unsigned int counter = 0;
            unsigned int timestamp;
            unsigned int current_timestamp = 0;
            string action;
            char T;
            uint16_t trader_id;
            uint32_t stock_id;
            int price;
            int quantity;
            int trade_price;
            int num_shares;

            while (inputStream >> timestamp >> action >> T >> trader_id >> T >> stock_id >> T >> price >> T >> quantity) {
                if(timestamp<current_timestamp||trader_id>=num_traders||stock_id>=num_stocks||price<=0||quantity<=0){
                    cerr << "invlid input";
                    exit(1);
                }

                Order order(counter, trader_id, price, quantity);

                if(timestamp!=current_timestamp&&median){
                    calculateMedian(current_timestamp);
                }

                if (action == "SELL"){
                    stockList[stock_id].sellQueue.push(order);
                }
                else{
                    stockList[stock_id].buyQueue.push(order);
                }

                if(time_travelers){
                    Stock &stock = stockList[stock_id];
                    if(action == "SELL"&&stock.time_traveler_status==Timestatus::NoTrade){
                        stock.buy_price=price;
                        stock.buy_timestamp=timestamp;
                        stock.time_traveler_status=Timestatus::CanBuy;
                    }
                    else if(action == "SELL"&&stock.time_traveler_status==Timestatus::CanBuy&&stock.buy_price>price){
                        stock.buy_price=price;
                        stock.buy_timestamp=timestamp;
                    }
                    else if(action == "BUY"&&stock.time_traveler_status==Timestatus::CanBuy&&price>stock.buy_price){
                        stock.sell_price=price;
                        stock.sell_timestamp=timestamp;
                        stock.time_traveler_status=Timestatus::Compeleted;

                    }
                    else if(action == "BUY"&&stock.time_traveler_status==Timestatus::Compeleted&&(price-stock.buy_price)>(stock.sell_price-stock.buy_price)){
                        stock.sell_price=price;
                        stock.sell_timestamp=timestamp;

                    }
                    else if(action == "SELL"&&stock.time_traveler_status==Timestatus::Compeleted){
                        stock.potential_price=price;
                        stock.potential_timestamp=timestamp;
                        stock.time_traveler_status=Timestatus::Potential;
                    }
                    else if(action == "SELL"&&stock.time_traveler_status==Timestatus::Potential&& price < stock.potential_price){
                        stock.potential_price=price;
                        stock.potential_timestamp=timestamp;
                    }
                    else if(action == "BUY"&&stock.time_traveler_status==Timestatus::Potential){
                        if((price-stock.potential_price)>(price - stock.buy_price)&&!((stock.sell_price-stock.buy_price)>=price-stock.potential_price)){
                            stock.buy_price = stock.potential_price;
                            stock.buy_timestamp = stock.potential_timestamp;
                            stock.sell_price=price;
                            stock.sell_timestamp=timestamp;
                            stock.time_traveler_status=Timestatus::Compeleted;
                        }
                        else if(price > stock.sell_price){
                            stock.sell_price=price;
                            stock.sell_timestamp=timestamp;
                        }
                    }
                }

                while(!stockList[stock_id].sellQueue.empty()&&!stockList[stock_id].buyQueue.empty()){
                    const Order &sell = stockList[stock_id].sellQueue.top();
                    const Order &buy = stockList[stock_id].buyQueue.top();
                    if(sell.price<=buy.price){
                        uint16_t selling_trader_id = sell.trader_id;
                        uint16_t buying_trader_id = buy.trader_id;

                        if(sell.ref<buy.ref){
                            trade_price = sell.price;
                        }
                        else{
                            trade_price = buy.price;
                        }
                        if(sell.quantity<buy.quantity){
                            num_shares = sell.quantity;
                            stockList[stock_id].buyQueue.top().quantity -= sell.quantity;
                            stockList[stock_id].sellQueue.pop();
                        }
                        else if(sell.quantity>buy.quantity){
                            num_shares = buy.quantity;
                            stockList[stock_id].sellQueue.top().quantity-= buy.quantity;
                            stockList[stock_id].buyQueue.pop();
                        }
                        else{
                            num_shares = buy.quantity;
                            stockList[stock_id].sellQueue.pop();
                            stockList[stock_id].buyQueue.pop();
                        }
                        if(verbose){
                            cout << "Trader " << buying_trader_id << " purchased " << num_shares <<" shares of Stock " << stock_id <<" from Trader " << selling_trader_id << " for $"<<trade_price<<"/share\n"; 
                        }

                        if(median){
                            insertMedian(trade_price, stock_id);
                        }

                        if(trader_info){
                            traderList[selling_trader_id].num_sold += num_shares;
                            traderList[selling_trader_id].net_value += num_shares*trade_price;
                            traderList[buying_trader_id].num_bought += num_shares;
                            traderList[buying_trader_id].net_value -= num_shares*trade_price;
                        }
                        trade_completed++;
                    }
                    else{
                        break;
                    }
                }
                current_timestamp = timestamp;
                counter++;
            }
            if(median){
                calculateMedian(current_timestamp);
            }  
        }

    public:
        Market() : num_traders(0), num_stocks(0){};

        void getMode(int argc, char * argv[]) {
            opterr = false; // Let us handle all error output for command line options
            int choice;
            int index = 0;
            option long_options[] = {
                {"verbose", no_argument, 0, 'v'},
                {"median", no_argument, 0, 'm'},
                {"trader_info", no_argument, 0, 'i'},
                {"time_travelers", no_argument, 0, 't'},
                { nullptr, 0,                 nullptr,  '\0'},
            }; 
            while ((choice = getopt_long(argc, argv, "vmit", long_options, &index)) != -1) {
                switch (choice) {
                    case 'v':
                        verbose = true;
                        break;
                    case 'm':
                        median = true;
                        break;
                    case 'i':
                        trader_info = true;
                        break;
                    case 't':
                        time_travelers = true;
                        break;
                    default:
                        cerr << "Usage: " << argv[0] << " [-v|--verbose] [-m|--median] [-i|--trader_info] [-t|--time_travelers]" << endl;
                        exit(EXIT_FAILURE);
                }
            }
        }

        void read(istream &inputStream){
            string comment;
            string mode;
            unsigned int seed;
            unsigned int num_orders;
            unsigned int rate;
            cout << "Processing orders...\n";
            getline(inputStream, comment);
            inputStream >> comment >> mode >> comment >> num_traders >> comment >> num_stocks;
            stockList.resize(static_cast<size_t>(num_stocks));

            if(trader_info){
                traderList.resize(static_cast<size_t>(num_traders));
            }
            
            stringstream ss;
            if (mode == "PR") {
                inputStream >> comment >> seed >> comment >> num_orders >> comment >> rate;
                P2random::PR_init(ss, seed, num_traders, num_stocks, num_orders, rate);
            }  
            if (mode == "PR"){
                processOrders(ss);
            } 
            else{
                processOrders(inputStream);
            }
            cout << "---End of Day---\n";
            cout << "Trades Completed: "<< trade_completed << "\n";
            if(trader_info){
                printTraderInfo();
            }
            if(time_travelers){
                printTimeTraveler();
            }
        }
};

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    Market market;
    market.getMode(argc, argv);
    market.read(cin);    
}  