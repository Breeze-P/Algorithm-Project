#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

struct StockTrade {
    int buyDay;
    int sellDay;
};

// 贪心算法实现
vector<StockTrade> greedyStockTrade(const vector<int>& prices, int maxTrades) {
    vector<StockTrade> trades;

    int n = prices.size();
    if (n < 2) {	// 边界条件处理，避免非法输入
        cout << "非法交易数据" << endl;
        return trades;
    }

    int tradesCount = 0;
    for (int i = 0; i < n - 1 && tradesCount < maxTrades; ++i) {
        if (prices[i + 1] > prices[i]) {
            StockTrade trade;
            trade.buyDay = i;
            while (i < n - 1 && prices[i + 1] > prices[i]) {
                ++i; // 找到价格峰值
            }
            trade.sellDay = i;
            trades.push_back(trade);
            ++tradesCount;
        }
    }

    return trades;
}

// 限制交易次数的动态规划
int DPStockTrade(const vector<int>& prices, int maxTrades) {
    vector<StockTrade> trades;

    int n = prices.size();
    if (n < 2) {	// 边界条件处理，避免非法输入
        cout << "非法交易数据" << endl;
        return -1;
    }

    // 创建动态规划数组，dp[i][k][0]表示第i天结束时不持有股票的最大利润，dp[i][k][1]表示第i天结束时持有股票的最大利润，k表示已完成的交易次数
    vector<vector<vector<int>>> dp(n, vector<vector<int>>(maxTrades + 1, vector<int>(2, 0)));

    for (int k = 1; k <= maxTrades; ++k) {
        dp[0][k][1] = -prices[0];
    }

    for (int i = 1; i < n; ++i) {
        for (int k = 1; k <= maxTrades; ++k) {
            // 状态转移方程
            dp[i][k][0] = max(dp[i - 1][k][0], dp[i - 1][k][1] + prices[i]);
            dp[i][k][1] = max(dp[i - 1][k][1], dp[i - 1][k - 1][0] - prices[i]);

            // 记录交易信息
            if (dp[i][k][0] > dp[i - 1][k][0] && dp[i][k][1] == dp[i - 1][k - 1][0]) {
                StockTrade trade;
                trade.buyDay = i - 1;
                trade.sellDay = i;
                trades.push_back(trade);
            }
        }
    }

    return dp[n - 1][maxTrades][0];
}

// 收费的动态规划
int DPStockTradeWithFee(const vector<int>& prices, int fee) {
    int n = prices.size();
    if (n < 2) {
        return -1;
    }

    // 创建动态规划数组，dp[i][0]表示第i天结束时不持有股票的最大利润，dp[i][1]表示第i天结束时持有股票的最大利润
    vector<vector<int>> dp(n, vector<int>(2, 0));
    // 记录每次交易的详细信息
    vector<StockTrade> trades;

    // 初始条件：第0天不持有股票的最大利润为0，持有股票的最大利润为-prices[0]
    dp[0][0] = 0;
    dp[0][1] = -prices[0] - fee; // 考虑手续费

    for (int i = 1; i < n; ++i) {
        // 状态转移方程
        dp[i][0] = max(dp[i - 1][0], dp[i - 1][1] + prices[i]);
        dp[i][1] = max(dp[i - 1][1], dp[i - 1][0] - prices[i] - fee); // 考虑手续费

        // 记录交易信息
        if (dp[i][0] > dp[i - 1][0] && dp[i][1] == dp[i - 1][0]) {
            StockTrade trade;
            trade.buyDay = i - 1;
            trade.sellDay = i;
            trades.push_back(trade);
        }
    }

    // return trades;
    return dp[n - 1][0];
}

// 有冷冻期的动态规划
int DPStockTradeWithCooldown(const vector<int>& prices) {
    int n = prices.size();
    if (n < 2) {
        return -1;
    }

    // 创建动态规划数组，dp[i][0]表示第i天结束时不持有股票的最大利润，dp[i][1]表示第i天结束时持有股票的最大利润
    vector<vector<int>> dp(n, vector<int>(2, 0));
    // 记录每次交易的详细信息
    vector<StockTrade> trades;

    // 初始条件：第0天不持有股票的最大利润为0，持有股票的最大利润为-prices[0]
    dp[0][0] = 0;
    dp[0][1] = -prices[0];

    for (int i = 1; i < n; ++i) {
        // 计算处于冷冻期的前一天的状态
        int prevDay = (i >= 2) ? dp[i - 2][0] : 0;

        // 状态转移方程
        dp[i][0] = max(dp[i - 1][0], dp[i - 1][1] + prices[i]);
        dp[i][1] = max(dp[i - 1][1], prevDay - prices[i]);

        // 记录交易信息
        if (dp[i][0] > dp[i - 1][0] && dp[i][1] == prevDay) {
            StockTrade trade;
            trade.buyDay = i - 1;
            trade.sellDay = i;
            trades.push_back(trade);
        }
    }

    // return trades;
    return dp[n - 1][0];
}

// 测试算法性能
void testAlgorithmPerformance(const vector<int>& data) {
    auto start_time = chrono::high_resolution_clock::now();

    // 调用算法函数
    int result = DPStockTrade(data, 10);

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    // 输出执行时间
    cout << "算法执行时间：" << duration.count() << " ms" << endl;
}

// 生成随机股价数据集
vector<int> generateRandomStockPrices(int size, int minPrice, int maxPrice) {
    vector<int> prices;

    // 设置随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> priceDist(minPrice, maxPrice);

    // 生成随机股价数据集
    for (int i = 0; i < size; ++i) {
        prices.push_back(priceDist(gen));
    }

    return prices;
}

int main() {
    // 示例股票价格数据
    // 用户输入股票价格数据
    // vector<int> prices;
    // int price;
    // cout << "输入股票价格序列（输入非字符值结束）：";
    // while (cin >> price) {
    //     prices.push_back(price);
    // }

    // 最大交易次数限制
    // int maxTrades = 1;

    // 贪心算法
    // vector<StockTrade> trades = DPStockTrade(prices);

    // 输出交易结果
    // if (trades.empty()) {
    //     cout << "非法交易" << endl;
    // } else {
    //     cout << "交易策略" << endl;
    //   	int totalProfit = 0;
    //     for (int i = 0; i < trades.size(); i++) {
    //         StockTrade trade = trades[i];
    //       	int profit = prices[trade.sellDay] - prices[trade.buyDay];
    //       	totalProfit += profit;
    //         cout << "购买日：" << trade.buyDay << "，卖出日：" << trade.sellDay << "，收益：" << profit << endl;
    //     }
    //   	cout << "总收益：" << totalProfit << endl;
    // }

    // 自动生成股票数据集
    vector<int> prices100 = generateRandomStockPrices(1000, 10, 200);
    vector<int> prices1000 = generateRandomStockPrices(10000, 10, 200);
    vector<int> prices10000 = generateRandomStockPrices(10000, 10, 200);
    cout << "100序列" << endl;
    testAlgorithmPerformance(prices100);
    cout << "1000序列" << endl;
    testAlgorithmPerformance(prices1000);
    cout << "10000序列" << endl;
    testAlgorithmPerformance(prices10000);

    return 0;
}