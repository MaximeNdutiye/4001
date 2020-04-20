### Maxime Ndutiye 101040367 SYSC 4100 A3

### How to run
The program can be compile using `make`.

To launch the **client** run the program with at least one argument
`make && ./record_store arg1`

To launch the **server** run the program with no arguments
`make && ./record_store`

I've kept the structure of the commands mostly intact so they should be
easy to follow. The commands are identical to instructions, and they all
take arguments as a space separated list. Here's some examples:

`insert mike accounting 1234 54321`
`check_employee_number mike`
`check_salary 1234`
`check accounting`
`delete 1234`

### Goals
The goal of this program is to allow for the storage of employee with
the following schema: `name, department name, employee number, salary`.
I chose to do this the simplest way possible; with an array.

The employee struct describes the schema. All the fields are strings to make
things easer. The maximum length of each field is also restricted to 12 characters
so as to not allow the buffer to overflow.

I chose to allow users to type entire commands and then verify them server side.
This allows for the client to be lighter since it does not have to also deal with checking for faults.

Insertions,lookups, and deletions are all straight forward since I used a simple array.

### Psedo code
The algorithm for the server starts out by finding which command was entered,
then we retreive the arguments. These steps are done with `strcmp` and `strtok`.

Here's the pseudo code for the various commands

**insert**
```
if (command == insert)
  create temporary employe

  copy arg1 into employee.name
  copy arg2 into employee.department_name
  copy arg3 into employee.employee_number
  copy arg4 into employee.salary

  save employee into array
```

**check_employee_number**
```
  store arg1 into employee_name

  while(current_emloyee.name is not employee_name)
    search for employee

    if (employee.name is employee_name)
      return employee.employee_number
```

**check_salary**
```
  store arg1 into employee_number

  while(current_emloyee.number is not employee_number)
    search for employee

    if (employee.employee_number is employee_number)
      return employee.salary
```

**check**
```
  store arg1 into department_name

  while(not all employees checked or number of found employees is 5)
    search for employee

    append employee.employee_number to string

    return string
```

**delete**
```
  store arg1 into employee_number

  while(current_emloyee.employe_enumber is not employee_number)
    search for employee

    if (employee.employee_number is employee_number)
      remove employee
      return 0

  if (employee not found)
    return -1
```

### Discussion
I ran the following commands and received the expected results

`insert mike accounting 1234 54321`
`insert jean accounting 4321 11111`

These return success

`check_employee_number mike`

returns 1234

`check_salary 1234`

returns 54321

`check accounting`

returns 1234 and 4321

`delete 1234`

returns 0

`check_employee_number mike`

returns not found