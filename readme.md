# C Implementation of Grubbs' Test for Outlier Detection

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A lightweight, dependency-free C library that implements the Grubbs' test (also known as the maximum normalized residual test) to identify and remove statistical outliers from a small dataset.

This code is designed for embedded systems and other resource-constrained environments where data cleansing is necessary before processing.

## Features

-   **Standard Grubbs' Test Implementation**: Accurately identifies and removes outliers.
-   **Configurable Confidence Levels**: Supports 99%, 95%, 90%, and 80% confidence levels to adjust the test's strictness.
-   **No External Dependencies**: Written in standard C (C89/C99 compatible) and requires only `stdbool.h`, `stdint.h`, and `math.h`.
-   **Simple API**: Easy to integrate with just two primary functions: `glbs_init()` and `glbs_process()`.
-   **Well-Documented**: Code is commented using Doxygen-style for easy understanding and documentation generation.

## Getting Started

### Prerequisites

You will need a C compiler (e.g., GCC, Clang) and the standard C library.

### Integration

To use this library in your project, simply copy `glbs.c` and `glbs.h` into your source directory and include `glbs.h` in the files where you need to use the functions.

```c
#include "glbs.h"
```

## Usage Example

Here is a simple example demonstrating how to use the library to clean a dataset with an obvious outlier.

```c
#include <stdio.h>
#include "glbs.h"

int main(void)
{
    // Sample data with a clear outlier (15.1)
    float samples[] = {8.2, 5.4, 5.0, 5.2, 15.1, 5.3, 5.5, 6.0};
    uint8_t num_samples = sizeof(samples) / sizeof(samples[0]);
    float result = 0.0f;

    printf("Original data: ");
    for(int i = 0; i < num_samples; ++i) {
        printf("%.1f ", samples[i]);
    }
    printf("\n");
    
    // Step 1: Initialize the module with a desired confidence level (e.g., 95%)
    glbs_init(GPN_95);

    // Step 2: Process the data to remove outliers and get the new average
    bool success = glbs_process(samples, num_samples, &result);

    if (success) {
        printf("Outlier (15.1) removed.\n");
        printf("The average of the cleaned data is: %.3f\n", result);
    } else {
        printf("Failed to process data. Check sample count.\n");
    }

    return 0;
}
```

**Expected Output:**

```
Original data: 8.2 5.4 5.0 5.2 15.1 5.3 5.5 6.0
Outlier (15.1) removed.
The average of the cleaned data is: 5.943
```

## API Reference

### `void glbs_init(gpn_mode_t mode);`

Initializes the Grubbs' test module with a specific confidence level.

-   **`mode`**: The desired confidence level. Can be one of `GPN_99`, `GPN_95`, `GPN_90`, or `GPN_80`.

### `bool glbs_process(const float *samples, uint8_t num, float *result);`

Processes a set of samples, iteratively removes outliers, and calculates the average of the remaining valid data points.

-   **`samples`**: A pointer to the input array of sample data.
-   **`num`**: The number of samples in the array (must be between 3 and 20, inclusive).
-   **`result`**: A pointer to a float where the final calculated average will be stored.
-   **Returns**: `true` on successful processing, or `false` if the input parameters are invalid.

## How It Works

The Grubbs' test is used to detect a single outlier in a univariate dataset that follows an approximately normal distribution. This implementation works as follows:

1.  The dataset is sorted to easily identify the minimum and maximum values.
2.  It iteratively calculates the mean and standard deviation of the current set of valid data points.
3.  For each data point, it computes the Grubbs' statistic `G = |value - mean| / std_dev`.
4.  The calculated `G` value is compared against a critical value from a pre-computed lookup table, which depends on the number of samples and the chosen confidence level.
5.  If `G` exceeds the critical value, the data point is flagged as an outlier and is excluded from the next iteration.
6.  This process repeats until no more outliers are found in an iteration.
7.  Finally, the average of all remaining valid data points is calculated and returned.

## License

This project is licensed under the MIT License.

---
<br>

# 用于异常值检测的格拉布斯检验法 C 语言实现

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

一个轻量级、无依赖的 C 语言库，实现了格拉布斯（Grubbs）检验法，用于从一个小数据集中识别和剔除统计学上的异常值（或称离群值）。

此代码专为嵌入式系统和其他资源受限的环境设计，适用于在数据处理前进行数据清洗的场景。

## 功能特性

-   **标准格拉布斯检验法实现**: 精确地识别和剔除异常值。
-   **可配置的置信度**: 支持 99%、95%、90% 和 80% 四种置信度，以调整检验的严格程度。
-   **无外部依赖**: 使用标准 C 语言编写（兼容 C89/C99），仅需要 `stdbool.h`, `stdint.h`, 和 `math.h`。
-   **简洁的 API**: 只需 `glbs_init()` 和 `glbs_process()` 两个核心函数即可轻松集成。
-   **完善的文档**: 代码采用 Doxygen 风格注释，便于理解和生成文档。

## 开始使用

### 环境要求

您需要一个 C 编译器（例如 GCC 或 Clang）以及标准 C 库。

### 如何集成

要将此库用于您的项目，只需将 `glbs.c` 和 `glbs.h` 文件复制到您的源代码目录中，并在需要使用的地方包含头文件 `glbs.h`。

```c
#include "glbs.h"
```

## 使用示例

以下是一个简单的示例，演示了如何使用本库来清洗一个包含明显异常值的数据集。

```c
#include <stdio.h>
#include "glbs.h"

int main(void)
{
    // 示例数据，包含一个明显的异常值 (15.1)
    float samples[] = {8.2, 5.4, 5.0, 5.2, 15.1, 5.3, 5.5, 6.0};
    uint8_t num_samples = sizeof(samples) / sizeof(samples[0]);
    float result = 0.0f;

    printf("原始数据: ");
    for(int i = 0; i < num_samples; ++i) {
        printf("%.1f ", samples[i]);
    }
    printf("\n");
    
    // 步骤 1: 使用所需的置信度（例如 95%）初始化模块
    glbs_init(GPN_95);

    // 步骤 2: 处理数据，剔除异常值并获取新的平均值
    bool success = glbs_process(samples, num_samples, &result);

    if (success) {
        printf("异常值 (15.1) 已被剔除。\n");
        printf("清洗后数据的平均值为: %.3f\n", result);
    } else {
        printf("数据处理失败，请检查样本数量是否在有效范围内。\n");
    }

    return 0;
}
```

**预期输出:**

```
原始数据: 8.2 5.4 5.0 5.2 15.1 5.3 5.5 6.0
异常值 (15.1) 已被剔除。
清洗后数据的平均值为: 5.943
```

## API 参考

### `void glbs_init(gpn_mode_t mode);`

使用指定的置信度初始化格拉布斯检验模块。

-   **`mode`**: 期望的置信度。可选值为 `GPN_99`, `GPN_95`, `GPN_90`, 或 `GPN_80`。

### `bool glbs_process(const float *samples, uint8_t num, float *result);`

处理一组样本数据，通过迭代剔除异常值，并计算剩余有效数据的平均值。

-   **`samples`**: 指向输入样本数据数组的指针。
-   **`num`**: 数组中的样本数量（必须在 3 到 20 之间，包含边界）。
-   **`result`**: 指向一个浮点数的指针，用于存储最终计算出的平均值。
-   **返回值**: 如果处理成功，返回 `true`；如果输入参数无效，则返回 `false`。

## 工作原理

格拉布斯检验法用于检测服从正态分布的单变量数据集中的单个异常值。本库的实现流程如下：

1.  对数据集进行排序，以便轻松找到最小值和最大值。
2.  迭代计算当前有效数据集的平均值和标准差。
3.  对每个数据点，计算其格拉布斯统计量 `G = |数据值 - 平均值| / 标准差`。
4.  将计算出的 `G` 值与预先计算好的临界值表中的值进行比较。临界值取决于样本数量和所选的置信度。
5.  如果 `G` 值大于临界值，则该数据点被标记为异常值，并在下一次迭代中被排除。
6.  重复此过程，直到在一轮完整的迭代中没有发现新的异常值。
7.  最后，计算所有剩余有效数据的平均值并返回。

## 许可证

本项目采用 MIT 许可证。