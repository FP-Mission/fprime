import {_loader} from "../loader.js";
import {config} from "../config.js"

Vue.component("b-testhistory", {
    template: "#testhistory-template",
    props:{
        filterText: {
            type: String,
            default: ""
        },
    },
    data: () => ({
        selected:"",
        tests:[],
    }),
    methods:{
        sendTest(){

        },
        async getTests(){
            this.tests = await (await fetch('http://127.0.0.1:5000/testall')).json();
        },
        columnify(item) {
            let date = new Date(item.date*1000)
            let orb = item.success ? config.dataSuccessIcon : config.dataErrorIcon;
            return [`${date.getDate()}.${date.getMonth()}.${date.getFullYear()} ${date.getHours()}:${date.getMinutes()}:${date.getSeconds()}`,
                item.test,`<img src=${orb} width="30" height="30"/>`];
        },
        /**
         * Take the given item and converting it to a unique key by merging the id and time together with a prefix
         * indicating the type of the item. Also strip spaces.
         * @param item: item to convert
         * @return {string} unique key
         */
        keyify(item) {
            return `${item.id}`;
        },
    },
    mounted(){
        this.getTests()
    }
    
  });