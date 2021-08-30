import {_loader} from "../loader.js";
import {config} from "../config.js"

Vue.component("b-test", {
    template: "#test-template",
    props:{
        filterText: {
            type: String,
            default: ""
        },
    },
    data: () => ({
        selected:"",
        tests:[],
        testHist:[]
    }),
    methods:{
        async sendTest(){
            let formData = new FormData();
            for(let i = 0 ; i < this.testHist.length; i++){
                if(this.testHist[i].test == this.selected){
                    this.testHist[i].success = 2;
                }
            }
            this.testHist = [...this.testHist];
            formData.append('script', this.selected);
            let res = await (await fetch("http://127.0.0.1:5000/test", {
            method: "post",
            body: formData
            })).json();
            await new Promise(r => setTimeout(r, 2000));
            this.getTests();

        },
        async getTests(){
            this.tests = await (await fetch('http://127.0.0.1:5000/test')).json();
            this.testHist = await (await fetch('http://127.0.0.1:5000/testhistory')).json();
        },
        columnify(item) {
            let date = new Date(item.date*1000)
            let orb = false;
            if(item.success == 2){
                orb = config.dataLoading;
            }else{
                orb = item.success ? config.dataSuccessIcon : config.dataErrorIcon;
            }
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
        historyTest(item){
            alert("sale")
        }
    },
    mounted(){
        this.getTests()
    }
    
  });