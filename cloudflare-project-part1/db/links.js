const fbIcon = require('simple-icons/icons/facebook');
const igIcon = require('simple-icons/icons/instagram');
const inIcon = require('simple-icons/icons/linkedin');


let linksArray = [
    { 'name': 'Link 1', 'url': 'https://link1url' },
    { 'name': 'Link 2', 'url': 'https://link2url' },
    { 'name': 'Link 3', 'url': 'https://link3url' }
]

let socialLinks = [
    { 'name': 'Instagram', 'url': 'https://instagram.com', 'svg': igIcon.svg },
    { 'name': 'Facebook', 'url': 'https://facebook.com', 'svg': fbIcon.svg },
    { 'name': 'LinkedIn','url': 'https://linkedin.com', 'svg': inIcon.svg }
]

const getJSONLinksArray = () => {
    return JSON.stringify(linksArray)
}

const getLinksArray = () => {
    return linksArray
}

const getSocialLinks = () => {
    return socialLinks
}

module.exports = {
    getJSONLinksArray,
    getLinksArray,
    getSocialLinks
}