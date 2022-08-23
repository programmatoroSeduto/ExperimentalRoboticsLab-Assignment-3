# TEMPLATE -- C++ code documentation

---

```{toctree}
---
maxdepth: 3
caption: Contents
---
./cpp-docs-template.md
```

---

## conventions

- the header contains the big part of the documentation
- the cpp file contains a reference to the documentation in the header
- **use the header documentation both for nodes and for header files**
- where possible, better to use javadoc supported by many other doc systems instead of the (horrible) doxygen statement, poorly supported except by doxygen itself. 
- **use HTML** to alter the markup inside the documentation text

## file documentation

documentation of a node or a library. 

- delete the unused fields
- use the name of the macro to make a reference to a interface
- remember to use `\ref` when you're referring to a name of the code

### Header documentation / node documentation

**use this both for nodes and for header files**

```html
/********************************************//**
 *  
 * \file file.ext
 * <div><b>ROS Node Name</b> 
 *      <ul><li>???ros_node???</li></ul></div>
 * \brief ...brief...
 * 
 * \authors ???
 * \version v1.0
 * 
 * <b>Description:</b> <br>
 * <p>
 * ...description
 * </p>
 * 
 * <b>UML component</b><br>
 * (See ... the overal architecture, for further informations)<br>
 * <img src="" alt="TODO uml"/><br>
 * 
 * <b>Publishers:</b> <br>
 * <ul>
 *     <li>
 * 			<i>/topic</i> : file.msg <br>
 * 			... description 
 * 		</li>
 * </ul>
 * 
 * <b>Subscribers:</b> <br>
 * <ul>
 *     <li>
 * 			<i>/topic</i> : file.msg <br>
 * 			... description 
 * 		</li>
 * </ul>
 * 
 * <b>Services:</b> <br>
 * <ul>
 *     <li>
 * 			<i>/service</i> : file.srv <br>
 * 			... description 
 * 		</li>
 * </ul>
 * 
 * <b>Clients:</b> <br>
 * <ul>
 *     <li>
 * 			<i>/serv</i> : file.srv <br>
 * 			... reference to the implementation
 * 		</li>
 * </ul>
 * 
 * <b>Providing actions</b> <br>
 * <ul>
 *     <li>
 * 			<i>action_name</i> : file.action <br>
 * 			... description 
 * 		</li>
 * </ul>
 * 
 * <b>Using actions</b> <br>
 * <ul>
 *     <li>
 * 			<i>action_name</i> : file.action <br>
 * 			... description 
 * 		</li>
 * </ul>
 * 
 * <b>Hidden Services and Topics:</b> <br>
 * <ul>
 * 		<li>
 * 			( from ... : type ) <i>/channel</i> : type.format <br>
 * 			... reference to page
 * 		</li>
 * </ul>
 * 
 * <b>Parameters:</b> <br>
 * <ul>
 * 		<li>
 * 			[GET/SET] <i>/parameter</i> : type <br>
 * 			... description 
 * 		</li>
 * </ul>
 * 
 * <b>Test the code</b><br>
 * <code>
 * ...
 * </code>
 * 
 * <b>TODOs</b><br>
 * 
 ***********************************************/
 ```
 
 ### Cpp file documentation
 
 this file simply makes a reference to the header. 
 
 
 ```html
 /********************************************//**
 *  
 * @file filename
 * @brief ...
 * 
 * @authors ...lista di autori separati da virgola...
 * @version v1.0
 * 
 * ... more details
 *  
 * @see header the Header
 * 
 ***********************************************/
```

## Code Elements Documentation

### naive doc

the most free template. 

```html
/********************************************//**
 *  
 * \brief ...
 * 
 * ... more details
 * 
 ***********************************************/
```

### classical function documentation

use this also for class methods!

```html
/********************************************//**
 *  
 * \brief ...
 * 
 * ... more details
 * 
 * @param param ... description
 * 
 * @returns ...description
 * 
 ***********************************************/
```

### MAIN FUNCTION documentation

```html
/********************************************//**
 *  
 * \brief ROS node main - 
 * 
 * ... more details
 * 
 ***********************************************/
```

### service implementation documentation

```html
/********************************************//**
 *  
 * \brief implementation of service \ref SERVICE_MACRO_OF_THE_SERVICE
 * 
 * ... more details
 * 
 * @param request ...description
 * @param response ...description
 * 
 * @see file.srv
 * 
 ***********************************************/
```

### publisher-callback documentation

this template can be used to document a function callback which also publishes something. 

```html
/********************************************//**
 *  
 * \brief subscriber to \ref SUBSCRIBER_HINT_SIGNAL and publisher of \ref PUBLISHER_HINT
 * 
 * ... more details
 * 
 * @param emptySignal empty 'request'
 * 
 * @see subscriber.msg [IN]
 * @see publisher.msg [OUT]
 * 
 ***********************************************/
```

## useful doxygen statements

see [doxygen statements](https://www.doxygen.nl/manual/commands.html)

see also [javadoc official giude](https://www.oracle.com/it/technical-resources/articles/java/javadoc-tool.html)

### TODO remarks

```
@todo something still to do
@bug describe the bug here ...
```

### annotations and remarks

```
\note
...your annotation...


\remark
...your annotation...

===AN EXAMPLE===
\note
This note consists of two paragraphs. <br>
This is the first paragraph.
```

### warnings and limitations

```text
@warning this is a warning
@deprecated ...deprecation notice here...

\attention
\warning
```
