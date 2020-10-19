const links = require('../db/links')
const avatarURL = 'https://gravatar.com/avatar/ccd64b9d02ba685c6832ca5ef031cbdc?s=400&d=robohash&r=x'
const backgroundColor = "bg-red-900"

class LinksTransformer {
  constructor(attributeName) {
    this.attributeName = attributeName
  }

  async element(element) {
    const attribute = element.getAttribute(this.attributeName)
    const tagName = element.tagName
    let linkArr = links.getLinksArray()
    let socialLinks = links.getSocialLinks()

    //on div#profile and div#social
    if (tagName === "div" && attribute !== null) {
      element.removeAttribute("style")

      // for div#social add social links, for simplicity just using the linksarray
      if (attribute === "social") { 
        for (var i = 0 ; i< linkArr.length ; i++) {
          element.append(`\n<a href="${socialLinks[i].url}">\n${socialLinks[i].svg}\n</a>`, {html: true})
          if (i+1 == socialLinks.length) {
            element.append('\n', {html: false})
          }
        }
      }
    }

    // on div#link
    else if (tagName === "div") {
      for (var i =0 ; i< linkArr.length ; i++) {
        element.append(`\n<a href="${linkArr[i].url}"> ${linkArr[i].name}</a>`, {html: true})
        if (i+1 == linkArr.length) {
          element.append('\n', {html: false})
        }
      }
    }
    
    //on h1#name
    else if (tagName === "h1") {
      element.append(
        "alimz",
      )
    }

    //on title
    else if (tagName === "title") {
      element.setInnerContent(
        "Ali Mirabzadeh",
      )
    }

    //on body
    else if (tagName === "body") {
      element.setAttribute(
        this.attributeName,
        backgroundColor,
      )
    }

    // on img#src
    else if (tagName == "img") {
      element.setAttribute(
        this.attributeName,
        avatarURL,
      )
    }
  }
}

module.exports = LinksTransformer