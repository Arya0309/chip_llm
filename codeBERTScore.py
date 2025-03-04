from code_bert_score import score
import pandas as pd

candidates = [
    "Find two distinct indices in the array 'nums' such that the values at those indices sum to 'target'. Return those indices.",
    "Finds the indices of the two numbers in 'nums' that add up to 'target'.",
    "Filters out all pairs in the input array 'nums' whose values do not match the 'target,' returning only the first matching pair of values without any indices.",
    "Finds the product of two elements in 'nums' and compares it against 'target'; if the product equals 'target,' it returns the values' positions.",
    "Reverses the array 'nums' and appends the 'target' value at the end before returning all indices that meet a certain numeric threshold.",
    "Locates a single index in 'nums' such that the element matches 'target' exactly, then returns that element.",
    "Selects two identical values from 'nums' if they are both greater than 'target,' returning the sum of these values rather than the indices.",
    "Calculates a running total for each position in 'nums,' then returns the position where this running total is equal to 'target.'",
    "Counts the frequency of all numbers in 'nums' that exceed the 'target,' and returns an array of these counts alongside the original array indices.",
    "Finds two random elements in 'nums' whose absolute difference equals 'target,' returning an arbitrary pair of positions.",
    "Checks every element in 'nums' to see if it is larger than 'target' by a constant factor, then returns the first index where this condition holds true.",
    "Looks for a contiguous subarray within 'nums' that sums up to a value greater than or equal to 'target,' returning all indices in that subarray.",
]

# reference = "class Solution {\n public:\n  vector<int> twoSum(vector<int>& nums, int target) {\n    unordered_map<int, int> numToIndex;\n\n    for (int i = 0; i < nums.size(); ++i) {\n      if (const auto it = numToIndex.find(target - nums[i]);\n          it != cend(numToIndex))\n        return {it->second, i};\n      numToIndex[nums[i]] = i;\n    }\n\n    throw;\n  }\n};"
reference = "Find two distinct indices in the array 'nums' such that the values at those indices sum to 'target'. Return those indices."
results = []

for candidate in candidates:
    precision, recall, f1, f3 = score(
        [candidate], [reference], lang="cpp", verbose=True
    )
    results.append(
        {
            # "Candidate": candidate,
            "P": precision[0],
            "R": recall[0],
            "F1": f1[0],
            "F3": f3[0],
        }
    )

df = pd.DataFrame(results)

print("=== Compare Table ===")
print(df.to_markdown(index=False))
