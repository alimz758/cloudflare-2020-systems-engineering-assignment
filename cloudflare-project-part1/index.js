
const links = require('./db/links')
const LinksTransformer = require('./LinksTransformer/LinksTransformer.js');
const staticHTMLURL = 'https://static-links-page.signalnerve.workers.dev'
const url = '/links';
const rewriter = new HTMLRewriter()
  .on("div#links", new LinksTransformer(""))
  .on("div#profile", new LinksTransformer("style"))
  .on("div#social", new LinksTransformer("id"))
  .on("h1#name", new LinksTransformer(""))
  .on("img#avatar", new LinksTransformer("src"))
  .on("title", new LinksTransformer(""))
  .on("body", new LinksTransformer("class"))
  

addEventListener('fetch', event => {
    event.respondWith(handleRequest(event.request))
})

// '/links' API handler
async function linksAPIhandler() {
    const init = {
        headers: { 
          'content-type': 'application/json',
          'Connection' : 'keep-alive'
        },
    }
    const body = links.getJSONLinksArray()

    return new Response(body, init)
}

//Helper function to send request to staticHTMLURL and get the HTML
async function htmlRequestHandler() {
  const init = {
    headers: {
      "content-type": "text/html;charset=UTF-8",
    },
  }
  const response = await fetch(staticHTMLURL, init)

  return rewriter.transform(response)
}

//Handle incoming requests
async function handleRequest(request) {
  let response 
  // '/links' API call
  if (request.url.slice(-6) === url) {
    response = await linksAPIhandler()
  }
  //any other links
  else {
    response =  await htmlRequestHandler()
  }

  return response
}