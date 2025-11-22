# def prompt_compile_error(generated_code: str, error_msg: str):
#     return [
#         {"role": "assistant", "content": generated_code},
#         {
#             "role": "user",
#             "content": (
#                 "The SystemC code you produced failed to compile:\n\n"
#                 f"{error_msg}\n\n"
#                 "Let’s think step-by-step (Chain-of-Thought):\n"
#                 "1. Briefly summarize the intended functionality of your original code.\n"
#                 "2. Identify each compilation error and the offending line or symbol.\n"
#                 "3. Explain the root causes and any deeper design flaws that allowed them.\n"
#                 "4. Outline a fresh, cleaner design that will avoid these problems.\n"
#                 "5. **Rewrite** the entire SystemC module from scratch, following your new design.\n\n"
#                 "Show your reasoning first, then present the complete, rewritten code."
#             ),
#         },
#     ]


# def prompt_runtime_error(generated_code: str, error_msg: str):

#     return [
#         {"role": "assistant", "content": generated_code},
#         {
#             "role": "user",
#             "content": (
#                 "Your SystemC code compiled but crashed or produced a runtime error:\n\n"
#                 f"{error_msg}\n\n"
#                 "Let’s analyze carefully (Chain-of-Thought):\n"
#                 "1. Describe the nature of the runtime failure (segfault, exception, wrong output, etc.).\n"
#                 "2. Locate where it occurs and why (stack trace, symptoms, logical mistake).\n"
#                 "3. Diagnose the underlying cause (e.g., null pointer, out-of-bounds, race condition).\n"
#                 "4. Formulate a redesigned approach that eliminates the issue.\n"
#                 "5. **Rewrite** the entire SystemC module from scratch using the improved design.\n\n"
#                 "Provide your reasoning first, then the full, rewritten code."
#             ),
#         },
#     ]


# def prompt_unit_test_error(generated_code: str, error_msg: str):
#     return [
#         {"role": "assistant", "content": generated_code},
#         {
#             "role": "user",
#             "content": (
#                 "Your SystemC code failed the unit tests:\n\n"
#                 f"{error_msg}\n\n"
#                 "Think through this step-by-step (Chain-of-Thought):\n"
#                 "1. List the failing test cases and describe the observed vs. expected behavior.\n"
#                 "2. Pinpoint which part of the code causes each failure.\n"
#                 "3. Explain the conceptual or logical errors behind those parts.\n"
#                 "4. Devise a corrected algorithm or structure that satisfies all tests.\n"
#                 "5. **Rewrite** a brand-new SystemC implementation from scratch to pass the tests.\n\n"
#                 "State your reasoning first, then provide the complete, rewritten code."
#             ),
#         },
#     ]


def prompt_compile_error(
    generated_code,
    error_msg,
):
    return [
        {"role": "assistant", "content": generated_code},
        {
            "role": "user",
            "content": (
                "The SystemC code you generated failed to compile with this error:\n\n"
                f"{error_msg}\n\n"
                "Let's debug step by step:\n"
                "1. Describe the compilation error and which line or symbol it references.\n"
                "2. Identify the code snippet or definition causing the compiler to complain.\n"
                "3. Explain why that part is invalid in SystemC/C++ (e.g., missing include, wrong type, syntax issue).\n"
                "4. Suggest precise edits (e.g., correct the syntax, add the missing declaration or include).\n"
                "5. Show the corrected code snippet.\n"
                "6. Finally, regenerate and provide the complete, fixed SystemC module code.\n\n"
                "Please walk through your reasoning before providing the fix."
            ),
        },
    ]


def prompt_runtime_error(
    generated_code,
    error_msg,
):
    return [
        {"role": "assistant", "content": generated_code},
        {
            "role": "user",
            "content": (
                "Your SystemC code compiled but crashed or threw a runtime error:\n\n"
                f"{error_msg}\n\n"
                "Let's analyze this carefully:\n"
                "1. Describe the nature of the runtime failure (segfault, exception, wrong output, etc.).\n"
                "2. Pinpoint the function or line where the error occurs (stack trace or symptoms).\n"
                "3. Explain the underlying cause (e.g., null pointer, out-of-bounds access, uninitialized variable).\n"
                "4. Propose the necessary code adjustments (e.g., add null checks, fix loop bounds, initialize variables).\n"
                "5. Provide the revised code section.\n"
                "6. Finally, regenerate and provide the complete, fixed SystemC module code.\n\n"
                "Detail your chain of thought before showing the fixed code."
            ),
        },
    ]


def prompt_unit_test_error(
    generated_code,
    error_msg,
):
    return [
        {"role": "assistant", "content": generated_code},
        {
            "role": "user",
            "content": (
                "Your SystemC code failed the unit tests with the following error:\n\n"
                f"{error_msg}\n\n"
                "Let's think through this step by step:\n"
                "1. Identify which test cases failed and describe the failure symptoms.\n"
                "2. Locate the part of the generated code that is causing the error.\n"
                "3. Explain why that section is incorrect or insufficient.\n"
                "4. Propose concrete changes to fix the issue.\n"
                "5. Provide the revised version of the code snippet.\n"
                "6. Finally, regenerate and provide the complete, fixed SystemC module code.\n\n"
                "Please show your reasoning before presenting the fixed code."
                " If it is empty, rewrite a new SystemC code snippet from scratch."
            ),
        },
    ]


def prompt_self_debugging_Expl(generated_code):
    return [
        {"role": "assistant", "content": generated_code},
        {
            "role": "user",
            "content": (
                "You just generated the SystemC module above. This code may be syntactically or semantically incorrect.\n\n"
                "Let's think through the code step by step:\n"
                "1. For each section (includes, port declarations, processes, methods, constructors, etc.), explain its purpose and how it should function in a correct SystemC/C++ design.\n"
                "2. Identify any issues—whether syntactic (e.g., missing headers, wrong syntax) or semantic (e.g., incorrect data types, flawed communication protocols, improper sensitivity lists).\n"
                "3. For each issue you find, explain why it is problematic, referencing the specific line or construct.\n"
                "4. Propose the exact edit needed to fix it (for example, add an `#include`, correct a type, adjust a sensitivity list).\n"
                "5. Show the corrected snippet for that part.\n"
                "6. If any errors were found, regenerate and provide the complete, corrected SystemC module code; otherwise, state that no changes are necessary and re-emit the original code.\n\n"
                "Please walk through your full reasoning before presenting any fixes or confirming the code."
            ),
        },
    ]
